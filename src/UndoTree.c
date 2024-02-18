#include <string.h>
#include <stdlib.h>

#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Event.h"

#include "Message.h"
#include "UndoTree.h"
#include "Format.h"
#include "Memory.h"
#include "MarkSelection.h"
#include "DataStructures/List.h"

typedef struct BlockChangeEntry_ {
    unsigned short x;
    unsigned short y;
    unsigned short z;
    DeltaBlockID delta;
} BlockChangeEntry;

typedef struct UndoNode_ UndoNode;

typedef struct UndoNode_ {
    int commit;
    char description[STRING_SIZE];
    BlockChangeEntry* blockDeltas;
    int blockDeltasCount;
    int blockDeltasCapacity;
    time_t timestamp;
    int parentIndex;
    int* children;
    size_t childrenCount;
    size_t childrenCapacity;
} UndoNode;

static bool s_Enabled = false;
static int s_CurrentNodeIndex = 0;

static UndoNode* s_Nodes = NULL;
static size_t s_NodesCount = 0;
static size_t s_NodesCapacity = 0;

static int* s_RedoStack = NULL;
static size_t s_RedoStackCapacity = 0;
static size_t s_RedoStackCount = 0;

static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block) {
    if (MarkSelection_RemainingMarks() > 0) {
        return;
    }

    if (block == BLOCK_AIR) {
        UndoTree_PrepareNewNode("Destroy");
    } else {
        UndoTree_PrepareNewNode("Place");
    }
 
    UndoTree_AddBlockChangeEntry(coords.X, coords.Y, coords.Z, block - oldBlock);
    UndoTree_Commit();
}

static void AddNode(UndoNode node) {
    if (s_NodesCount >= s_NodesCapacity) {
        size_t newCapacity = (s_NodesCapacity + 1) * 2;
        UndoNode* newNodes = Memory_Reallocate(s_Nodes, newCapacity * sizeof(UndoNode));

        s_NodesCapacity = newCapacity;
        s_Nodes = newNodes;
    }

    s_Nodes[s_NodesCount] = node;
    s_NodesCount += 1;
    return;
}

bool UndoTree_Enable(void) {
    if (s_Enabled) {
        return false;
    }

    UndoNode root;
    root.commit = 0;
    root.description[0] = '#';
    root.description[1] = '\0';
    root.blockDeltasCount = 0;
    root.blockDeltasCapacity = 0;
    root.blockDeltas = NULL;
    root.timestamp = time(NULL);
    root.parentIndex = 0;
    root.children = NULL;
    root.childrenCount = 0;
    root.childrenCapacity = 0;

    AddNode(root);

    Event_Register((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)OnBlockChanged);
    s_Enabled = true;

    return true;
}

static void FreeUndoNode(UndoNode* node) {
    free(node->blockDeltas);
    free(node->children);
}

void UndoTree_Disable(void) {
    if (!s_Enabled) {
        return;
    }

    for (size_t i = 0; i < s_NodesCount; i++) {
        FreeUndoNode(&s_Nodes[i]);
    }

    s_CurrentNodeIndex = 0;

    free(s_Nodes);
    s_Nodes = NULL;
    s_NodesCount = 0;
    s_NodesCapacity = 0;

    free(s_RedoStack);
    s_RedoStack = NULL;
    s_RedoStackCount = 0;
    s_RedoStackCapacity = 0;

    Event_Unregister((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)OnBlockChanged);
    s_Enabled = false;
}

static void Descend(UndoNode* child) {
    // Note: `child` must be a direct child of `s_CurrentNodeIndex`, otherwise the behaviour is undefined.
    s_CurrentNodeIndex = child->commit;
    BlockChangeEntry* entries = s_Nodes[s_CurrentNodeIndex].blockDeltas;
    BlockID currentBlock; 

    for (int i = 0; i < s_Nodes[s_CurrentNodeIndex].blockDeltasCount; i++) {
        currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
        Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock + entries[i].delta);
    }
}

static void Ascend(void) {
    if (s_CurrentNodeIndex == 0) {
        return;
    }

    BlockChangeEntry* entries = s_Nodes[s_CurrentNodeIndex].blockDeltas;
    BlockID currentBlock; 

    for (int i = s_Nodes[s_CurrentNodeIndex].blockDeltasCount - 1; i >= 0; i--) {
        currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
        Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock - entries[i].delta);
    }

    s_CurrentNodeIndex = s_Nodes[s_CurrentNodeIndex].parentIndex;
}

static bool Checkout(int target) {
    // Calculate the ancestors of the target.
    List* targetAncestors = List_CreateEmpty();

    UndoNode* ancestor = &s_Nodes[target];

    while (true) {
        List_Append(targetAncestors, ancestor);

        if (ancestor->commit == 0) {
            break;
        }

        ancestor = &s_Nodes[ancestor->parentIndex];
    }

    // Ascend while the current node is not an ancestor of the target.
    while (!List_Contains(targetAncestors, &s_Nodes[s_CurrentNodeIndex])) {
        Ascend();
    }

    // Remove all ancestors above `s_CurrentNodeIndex` until last element of `targetAncestors` is a child of `s_CurrentNodeIndex`.
    do {
        ancestor = (UndoNode*) List_Pop(targetAncestors);
    } while (ancestor->commit != s_CurrentNodeIndex);

    // Then, descend to the target.
    while (s_CurrentNodeIndex != target) {
        Descend((UndoNode*) List_Pop(targetAncestors));
    }

    List_Free(targetAncestors);
    return true;
}

static void StackRedo(int commit) {
    if (s_RedoStackCount >= s_RedoStackCapacity) {
        size_t newCapacity = (s_RedoStackCapacity + 1) * 2;
        int* newRedoStack = Memory_Reallocate(s_RedoStack, newCapacity * sizeof(int));

        s_RedoStackCapacity = newCapacity;
        s_RedoStack = newRedoStack;
    }

    s_RedoStack[s_RedoStackCount] = commit;
    s_RedoStackCount++;
}

static void ShowCurrentNode(void) {
    char messageBuffer[STRING_SIZE];
    cc_string message = { messageBuffer, .length = 0, .capacity = STRING_SIZE };
    UndoTree_FormatCurrentNode(&message);
    Chat_AddOf(&message, MSG_TYPE_SMALLANNOUNCEMENT);
}

void UndoTree_Earlier(int deltaTimeSeconds) {
    if (!s_Enabled) {
        return;
    }

    if (deltaTimeSeconds <= 0) {
        return;
    }

    int newIndex = 0;

    for (int i = s_CurrentNodeIndex; i > 0; i--) {
        if (s_Nodes[s_CurrentNodeIndex].timestamp - s_Nodes[i].timestamp > deltaTimeSeconds) {
            newIndex = i;
            break;
        }
    }

    if (newIndex == s_CurrentNodeIndex) {
        Message_Player("Already at the earliest moment.");
        return;
    }

    StackRedo(s_CurrentNodeIndex);
    Checkout(newIndex);
    ShowCurrentNode();
}

void UndoTree_Later(int deltaTimeSeconds) {
    if (!s_Enabled) {
        return;
    }

    if (deltaTimeSeconds <= 0) {
        return;
    }

    time_t targetTimestamp = s_Nodes[s_CurrentNodeIndex].timestamp + (long)deltaTimeSeconds;

    int lastNodeIndex = s_NodesCount - 1;
    int newIndex = s_CurrentNodeIndex;

    for (int i = lastNodeIndex; i > s_CurrentNodeIndex; i--) {
        if ((s_Nodes[i].timestamp < targetTimestamp)) {
            newIndex = (int)i;
            break;
        }
    }

    if (newIndex == s_CurrentNodeIndex) {
        Message_Player("No operation was found within this timespan.");
        return;
    }

    StackRedo(s_CurrentNodeIndex);
    Checkout(newIndex);
    ShowCurrentNode();
}

void UndoTree_Undo(void) {
    if (!s_Enabled) {
        return;
    }

    if (s_CurrentNodeIndex == 0) {
        Message_Player("There is nothing to undo.");
        return;
    }

    StackRedo(s_CurrentNodeIndex);
    Ascend();
    ShowCurrentNode();
}

void UndoTree_Checkout(int commit) {
    if (!s_Enabled) {
        return;
    }

    if (commit < 0 || s_NodesCount <= (size_t)commit) {
        char operationBuffer[STRING_SIZE];
        cc_string operation = String_FromArray(operationBuffer);
        Format_Int32(&operation, commit);

        char operationNotFoundBuffer[STRING_SIZE];
        cc_string operationNotFound = String_FromArray(operationNotFoundBuffer);
        String_Format1(&operationNotFound, "There is no operation &b%s&f.", &operation);

        Chat_Add(&operationNotFound);
        return;
    }

    StackRedo(s_CurrentNodeIndex);
    Checkout(commit);
    ShowCurrentNode();
}

void UndoTree_Redo(void) {
    if (!s_Enabled) {
        return;
    }

    if (s_RedoStackCount == 0) {
        Message_Player("You have nothing to redo.");
        return;
    }

    int nodeIndexTarget = s_RedoStack[s_RedoStackCount - 1];
    s_RedoStackCount--;
    Checkout(nodeIndexTarget);
    ShowCurrentNode();
}

static void AddChildren(void) {
    UndoNode* here = &s_Nodes[s_CurrentNodeIndex];

    if (here->childrenCount >= here->childrenCapacity) {
        size_t newCapacity = (here->childrenCapacity + 1) * 2;
        int* newChildren = Memory_Reallocate(here->children, newCapacity * sizeof(int));

        here->childrenCapacity = newCapacity;
        here->children = newChildren;
    }

    here->children[here->childrenCount] = s_NodesCount;
    here->childrenCount++;
}

bool UndoTree_PrepareNewNode(char* description) {
    if (!s_Enabled) {
        return false;
    }

    UndoNode newNode;

    newNode.commit = s_NodesCount;
    strncpy(newNode.description, description, sizeof(newNode.description));
    newNode.blockDeltas = NULL;
    newNode.blockDeltasCount = 0;
    newNode.blockDeltasCapacity = 0;
    newNode.parentIndex = s_CurrentNodeIndex;
    newNode.children = NULL;
    newNode.childrenCount = 0;
    newNode.childrenCapacity = 0;
    newNode.timestamp = time(NULL);

    AddNode(newNode);
    AddChildren();

    s_CurrentNodeIndex = newNode.commit;
    return true;
}

void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta) {
    if (!s_Enabled) {
        return;
    }

    BlockChangeEntry entry = {
        .x = x,
        .y = y,
        .z = z,
        .delta = delta
    };

    s_Nodes[s_CurrentNodeIndex].blockDeltasCount += 1;

    if (s_Nodes[s_CurrentNodeIndex].blockDeltasCount > s_Nodes[s_CurrentNodeIndex].blockDeltasCapacity) {
        s_Nodes[s_CurrentNodeIndex].blockDeltasCapacity = (s_Nodes[s_CurrentNodeIndex].blockDeltasCapacity + 1) * 2;

        BlockChangeEntry* newEntries = Memory_Reallocate(s_Nodes[s_CurrentNodeIndex].blockDeltas, s_Nodes[s_CurrentNodeIndex].blockDeltasCapacity * sizeof(BlockChangeEntry));
        s_Nodes[s_CurrentNodeIndex].blockDeltas = newEntries;
    }

    s_Nodes[s_CurrentNodeIndex].blockDeltas[s_Nodes[s_CurrentNodeIndex].blockDeltasCount - 1] = entry;
}

void UndoTree_Commit(void) {
    if (!s_Enabled) {
        return;
    }

    if (s_Nodes[s_CurrentNodeIndex].blockDeltasCount == 0) {
        s_NodesCount--;
        s_CurrentNodeIndex = s_Nodes[s_CurrentNodeIndex].parentIndex;
        return;
    }

    free(s_RedoStack);
    s_RedoStack = NULL;
    s_RedoStackCount = 0;
    s_RedoStackCapacity = 0;
}

static void FormatNode(cc_string* destination, UndoNode* node) {
    char buffer_result[STRING_SIZE];
    cc_string result = String_FromArray(buffer_result);

    char buffer_currentNodePrefix[] = { '&', 'b', 0x10, ' ', '&', 'f', '\0' };
    cc_string currentNodePrefix = { buffer_currentNodePrefix, .length = 6, .capacity = 6 };

    char buffer_formattedTime[STRING_SIZE];
    cc_string formattedTime = String_FromArray(buffer_formattedTime);

    char buffer_formattedBlocks[STRING_SIZE];
    cc_string formattedBlocks = String_FromArray(buffer_formattedBlocks);

    char buffer_formattedCommit[STRING_SIZE];
    cc_string formattedCommit = String_FromArray(buffer_formattedCommit);

    Format_HHMMSS(&formattedTime, node->timestamp);
    Format_Int32(&formattedBlocks, node->blockDeltasCount);
    Format_Int32(&formattedCommit, node->commit);

    String_Format4(&result, "[&b%s&f] %c @ %s (%s)", &formattedCommit, node->description, &formattedTime, &formattedBlocks);

    if (node->commit == s_CurrentNodeIndex) {
        String_AppendString(destination, &currentNodePrefix);
    }

    String_AppendString(destination, &result);
}

void UndoTree_UndoList(cc_string* descriptions, int* count) {
    const int max = 5;

    UndoNode* terminalNodes[max];
    *count = 0;

    for (int i = s_NodesCount - 1; i >= 0; i--) {
        if (s_Nodes[i].childrenCount == 0) {
            terminalNodes[*count] = &s_Nodes[i];
            *count += 1;
        }

        if (*count == max) {
            break;
        }
    }

    UndoNode* currentNode;

    for (int i = 0; i < *count; i++) {
        currentNode = terminalNodes[i];
        FormatNode(&descriptions[i], currentNode);
    }
}

long UndoTree_CurrentTimestamp(void) {
    return s_Nodes[s_CurrentNodeIndex].timestamp;
}

void UndoTree_FormatCurrentNode(cc_string* destination) {
    UndoNode* currentNode = &s_Nodes[s_CurrentNodeIndex];
    FormatNode(destination, currentNode);
}

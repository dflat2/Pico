#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Constants.h"

#include "Messaging.h"
#include "UndoTree.h"
#include "Format.h"
#include "MarkSelection.h"
#include "DataStructures/List.h"

typedef struct BlockChangeEntry_ {
	int x;
	int y;
	int z;
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
	UndoNode* parent;
	List* children;
} UndoNode;

static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block);
static void Checkout(UndoNode* target, int* ascended, int* descended);
static void Descend(UndoNode* child);
static void Ascend(void);
static void FreeUndoNode(UndoNode* node);
static bool TryInitRoot(void);
static UndoNode* HistoryTryAdd(void);

static UndoNode* s_Here = NULL;
static UndoNode* s_BuildingNode = NULL;
static bool s_Enabled = false;

static UndoNode* s_Nodes = NULL;
static size_t s_NodesCount = 0;
static size_t s_NodesCapacity = 0;

static List* s_RedoStack = NULL;

bool UndoTree_Enable(void) {
	if (s_Enabled) {
		return false;
	}

	if (!TryInitRoot()) {
		return false;
	}

	s_Here = &s_Nodes[0];
	s_BuildingNode = NULL;
	s_RedoStack = List_CreateEmpty();

	if (s_RedoStack == NULL) {
		FreeUndoNode(&s_Nodes[0]);
		free(s_Nodes);
		s_NodesCount = 0;
		s_NodesCapacity = 0;
		return false;
	}

    Event_Register((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)OnBlockChanged);
	s_Enabled = true;
	return true;
}

void UndoTree_Disable(void) {
	if (!s_Enabled) {
		return;
	}

	for (int i = 0; i < s_NodesCount; i++) {
		FreeUndoNode(&s_Nodes[i]);
	}

	free(s_Nodes);
	s_Nodes = NULL;
	s_NodesCapacity = 0;
	s_NodesCount = 0;

	List_Free(s_RedoStack);
    Event_Unregister((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)OnBlockChanged);

	s_Enabled = false;
}

bool UndoTree_Enabled(void) {
	return s_Enabled;
}

bool UndoTree_Earlier(int deltaTimeSeconds, int* out_commit) {
	if (!s_Enabled) {
		return false;
	}

	if (deltaTimeSeconds <= 0) {
		return false;
	}

	int newIndex = 0;

	for (int i = s_Here->commit - 1; i > 0; i--) {
		if (s_Here->timestamp - s_Nodes[i].timestamp > deltaTimeSeconds) {
			newIndex = i;
			break;
		}
	}

	if (newIndex == s_Here->commit) {
		return false;
	}

	List_Append(s_RedoStack, s_Here);
	Checkout(&s_Nodes[newIndex], NULL, NULL);
	*out_commit = s_Here->commit;
	return true;
}

bool UndoTree_Later(int deltaTimeSeconds, int* commit) {
	if (!s_Enabled) {
		return false;
	}

	if (deltaTimeSeconds <= 0) {
		return false;
	}

	int newIndex = s_NodesCount;

	for (int i = s_Here->commit; i < s_NodesCount - 1; i++) {
		if (s_Nodes[i].timestamp - s_Here->timestamp > deltaTimeSeconds) {
			newIndex = i;
			break;
		}
	}

	if (newIndex == s_Here->commit) {
		return false;
	}

	List_Append(s_RedoStack, s_Here);
	Checkout(&s_Nodes[newIndex], NULL, NULL);
	*commit = s_Here->commit;
	return true;
}

bool UndoTree_Undo(void) {
	if (!s_Enabled || s_Here->parent == NULL) {
		return false;
	}

	List_Append(s_RedoStack, s_Here);
	Ascend();
	return true;
}

bool UndoTree_Checkout(int commit, int* ascended, int* descended) {
	if (!s_Enabled) {
		return false;
	}

	if (0 < commit || s_NodesCount <= commit) {
		return false;
	}

	List_Append(s_RedoStack, s_Here);
	Checkout(&s_Nodes[commit], ascended, descended);
	return true;
}

bool UndoTree_Redo(void) {
	if (!s_Enabled || List_Count(s_RedoStack) == 0) {
		return false;
	}

	UndoNode* target = List_Pop(s_RedoStack);
	Checkout(target, NULL, NULL);
	return true;
}

bool UndoTree_TryPrepareNewNode(char* description) {
	if (!s_Enabled) {
		return false;
	}

	s_BuildingNode = HistoryTryAdd();

	if (s_BuildingNode == NULL) {
		return false;
	}
	
	s_BuildingNode->children = List_CreateEmpty();

	if (s_BuildingNode->children == NULL) {
		s_NodesCount--;
		return false;
	}

	s_BuildingNode->commit = s_NodesCount - 1;
	strncpy(s_BuildingNode->description, description, sizeof(s_BuildingNode->description));
	s_BuildingNode->blockDeltas = NULL;
	s_BuildingNode->blockDeltasCount = 0;
	s_BuildingNode->blockDeltasCapacity = 0;
	s_BuildingNode->timestamp = time(NULL);
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

	s_BuildingNode->blockDeltasCount += 1;

	if (s_BuildingNode->blockDeltasCount > s_BuildingNode->blockDeltasCapacity) {
		s_BuildingNode->blockDeltasCapacity = (s_BuildingNode->blockDeltasCapacity + 1) * 2;

		BlockChangeEntry* newEntries = realloc(s_BuildingNode->blockDeltas, s_BuildingNode->blockDeltasCapacity * sizeof(BlockChangeEntry));

		if (newEntries == NULL) {
			// TODO: Don't do that
			exit(1);
		}

		s_BuildingNode->blockDeltas = newEntries;
	}

	s_BuildingNode->blockDeltas[s_BuildingNode->blockDeltasCount - 1] = entry;
}

void UndoTree_Commit(void) {
	if (!s_Enabled) {
		return;
	}

	if (s_BuildingNode->blockDeltasCount == 0) {
		s_NodesCount--;
		s_BuildingNode = NULL;
		return;
	}

	List_Append(s_Here->children, s_BuildingNode);
	s_BuildingNode->parent = s_Here;
	s_Here = s_BuildingNode;
	List_Clear(s_RedoStack);
	s_BuildingNode = NULL;
}

void UndoTree_UndoList(cc_string* descriptions, int* count) {
	const int max = 5;

	UndoNode* terminalNodes[max];
	*count = 0;

	for (int i = s_NodesCount - 1; i > 0; i--) {
		if (List_Count(s_Nodes[i].children) == 0) {
			terminalNodes[*count] = &s_Nodes[i];
			*count += 1;
		}

		if (*count == max) {
			break;
		}
	}

	char buffer_formattedTime[STRING_SIZE];
	cc_string formattedTime = String_FromArray(buffer_formattedTime);

	char buffer_formattedBlocks[STRING_SIZE];
	cc_string formattedBlocks = String_FromArray(buffer_formattedBlocks);

	char buffer_formattedCommit[STRING_SIZE];
	cc_string formattedCommit = String_FromArray(buffer_formattedCommit);

	UndoNode* currentNode;

	for (int i = 0; i < *count; i++) {
		currentNode = terminalNodes[i];

		Format_HHMMSS(&formattedTime, currentNode->timestamp);
		Format_Int32(&formattedBlocks, currentNode->blockDeltasCount);
		Format_Int32(&formattedCommit, currentNode->commit);

		if (currentNode->blockDeltasCount == 1) {
			String_Format4(&descriptions[i], "[&b%s&f] %c @ %s (%s block)", &formattedCommit, currentNode->description, &formattedTime, &formattedBlocks);
		} else {
			String_Format4(&descriptions[i], "[&b%s&f] %c @ %s (%s blocks)", &formattedCommit, currentNode->description, &formattedTime, &formattedBlocks);
		}
	}
}

long UndoTree_CurrentTimestamp(void) {
	return s_Here->timestamp;
}

static bool TryInitRoot(void) {
	UndoNode* root = HistoryTryAdd();

	if (root == NULL) {
		return false;
	}

	root->commit = 0;
	char description[] = "World loaded";
	strncpy(root->description, description, sizeof(description));
	root->blockDeltasCount = World.Width * World.Height * World.Length;
	root->blockDeltas = NULL;
	root->timestamp = time(NULL);
	root->parent = NULL;
	root->children = List_CreateEmpty();

	if (root->children == NULL) {
		free(s_Nodes);
		s_NodesCapacity = 0;
		s_NodesCount = 0;
		return false;
	}

	return true;
}

static void FreeUndoNode(UndoNode* node) {
	free(node->blockDeltas);
	List_Free(node->children);
}

static void Ascend(void) {
	BlockChangeEntry* entries = s_Here->blockDeltas;
	BlockID currentBlock; 

	for (int i = 0; i < s_Here->blockDeltasCount; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock - entries[i].delta);
	}

	s_Here = s_Here->parent;
}

static void Descend(UndoNode* child) {
	// Note: `child` must be a direct child of `s_Here`, otherwise the behaviour is undefined.
	s_Here = child;
	BlockChangeEntry* entries = s_Here->blockDeltas;
	BlockID currentBlock; 

	for (int i = 0; i < s_Here->blockDeltasCount; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock + entries[i].delta);
	}
}

static void Checkout(UndoNode* target, int* ascended, int* descended) {
	if (ascended != NULL && descended != NULL) {
		*ascended = 0;
		*descended = 0;
	}

	// Calculate the ancestors of the target.
	List* targetAncestors = List_CreateEmpty();
	UndoNode* ancestor = target;

	do {
		List_Append(targetAncestors, ancestor);
		ancestor = ancestor->parent;
	} while (ancestor != NULL);

	// Ascend while the current node is not an ancestor of the target.
	while (!List_Contains(targetAncestors, s_Here)) {
		Ascend();
		
		if (ascended != NULL) {
			(*ascended)++;
		}
	}

	// Remove all ancestors above `s_Here` until last element of `targetAncestors` is a child of `s_Here`.
	while (s_Here != (UndoNode*) List_Pop(targetAncestors));

	// Then, descend to the target.
	while (s_Here != target) {
		Descend((UndoNode*) List_Pop(targetAncestors));
		
		if (descended != NULL) {
			(*descended)++;
		}
	}

	List_Free(targetAncestors);
}

static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block) {
	if (MarkSelection_RemainingMarks() > 0) {
		return;
	}

	if (block == BLOCK_AIR) {
		UndoTree_TryPrepareNewNode("Destroy");
	} else {
		UndoTree_TryPrepareNewNode("Place");
	}
 
	UndoTree_AddBlockChangeEntry(coords.X, coords.Y, coords.Z, block - oldBlock);
	UndoTree_Commit();
}

static UndoNode* HistoryTryAdd(void) {
	if (s_NodesCount >= s_NodesCapacity) {
		size_t newCapacity = (s_NodesCapacity + 1) * 2;

		UndoNode* newNodes = realloc(s_Nodes, newCapacity * sizeof(UndoNode));

		if (newNodes == NULL) {
			return NULL;
		}

		s_NodesCapacity = newCapacity;
		s_Nodes = newNodes;
	}

	s_NodesCount += 1;
	return &s_Nodes[s_NodesCount - 1];
}
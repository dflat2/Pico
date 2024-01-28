#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Event.h"

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
	char description[64];
	int blocksAffected;
	BlockChangeEntry* entries;
	time_t timestamp;
	UndoNode* parent;
	List* children;
	UndoNode* redoChild;
} UndoNode;

static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block);
static void SetRedoChild(UndoNode* node);
static void Attach(UndoNode* parent, UndoNode* child);
static void CheckoutFromNode(UndoNode* target, int* ascended, int* descended);
static void Ancestors(UndoNode* node, List* out_ancestors);
static UndoNode* FindNode(int commit);
static void Descend(void);
static void Ascend(void);
static bool BuildingNode(void);
static void FreeUndoNode(UndoNode* node);
static bool TryInitRoot(void);
static void GetLeaves(List* out_leaves);

static UndoNode s_Root;
static UndoNode* s_Here = NULL;
static UndoNode* s_BuildingNode = NULL;
static int s_EntriesSize = 1;
static int s_CommitAutoIncrement = 0;
static bool s_Enabled = false;
static List* s_History = NULL;
static List* s_RedoStack = NULL;

void UndoTree_Enable(void) {
	if (s_Enabled) return;

	TryInitRoot();
	s_Here = &s_Root;
	s_BuildingNode = NULL;
	s_CommitAutoIncrement = 0;
	s_History = List_CreateEmpty();
	List_Append(s_History, &s_Root);
	s_RedoStack = List_CreateEmpty();

	s_Enabled = true;

    struct Event_Void* event = (struct Event_Void*) &UserEvents.BlockChanged;
    Event_Void_Callback callback = (Event_Void_Callback)OnBlockChanged;
    Event_Register(event, NULL, callback);
}

void UndoTree_Disable(void) {
	if (!s_Enabled) return;

	UndoNode* currentNode = NULL;

	while (!List_IsEmpty(s_History)) {
		currentNode = (UndoNode*) List_Pop(s_History);
		FreeUndoNode(currentNode);
	}

	if (s_BuildingNode != NULL) {
		FreeUndoNode(s_BuildingNode);
		s_BuildingNode = NULL;
	}

	List_Free(s_History);
	List_Free(s_RedoStack);
	s_Enabled = false;
	Message_MessageOf("", MSG_TYPE_STATUS_1);
	Message_MessageOf("", MSG_TYPE_STATUS_2);

    struct Event_Void* event = (struct Event_Void*) &UserEvents.BlockChanged;
    Event_Void_Callback callback = (Event_Void_Callback)OnBlockChanged;
    Event_Unregister(event, NULL, callback);
}

bool UndoTree_Enabled(void) {
	return s_Enabled;
}

bool UndoTree_Earlier(int deltaTimeSeconds, int* commit) {
	if (!s_Enabled || BuildingNode()) return false;
	if (deltaTimeSeconds <= 0) return false;

	int historyIndex = List_IndexOf(s_History, s_Here);

	int newIndex = -1;
	UndoNode* currentNode;

	for (int i = historyIndex - 1; i > 0; i--) {
		currentNode = (UndoNode*) List_Get(s_History, i);
		if (s_Here->timestamp - currentNode->timestamp > deltaTimeSeconds) {
			newIndex = i;
			break;
		}
	}

	if (newIndex < 0) {
		newIndex = 0;
	}

	if (newIndex == historyIndex) return false;

	UndoNode* target = List_Get(s_History, newIndex);
	List_Append(s_RedoStack, s_Here);
	CheckoutFromNode(target, NULL, NULL);
	*commit = s_Here->commit;
	return true;
}

bool UndoTree_Later(int deltaTimeSeconds, int* commit) {
	if (!s_Enabled || BuildingNode()) return false;
	if (deltaTimeSeconds <= 0) return false;

	int historyIndex = List_IndexOf(s_History, s_Here);

	int count = List_Count(s_History);
	int newIndex = count;
	UndoNode* currentNode;

	for (int i = historyIndex + 1; i < count - 1; i++) {
		currentNode = (UndoNode*) List_Get(s_History, i);
		if (currentNode->timestamp - s_Here->timestamp > deltaTimeSeconds) {
			newIndex = i;
			break;
		}
	}

	if (newIndex >= count) {
		newIndex = count - 1;
	}

	if (newIndex == historyIndex) return false;

	UndoNode* target = List_Get(s_History, newIndex);
	List_Append(s_RedoStack, s_Here);
	CheckoutFromNode(target, NULL, NULL);
	*commit = s_Here->commit;
	return true;
}

bool UndoTree_Undo(void) {
	if (!s_Enabled || BuildingNode() || s_Here->parent == NULL) return false;

	List_Append(s_RedoStack, s_Here);
	Ascend();
	return true;
}

bool UndoTree_Checkout(int commit, int* ascended, int* descended) {
	if (!s_Enabled || BuildingNode()) return false;

    UndoNode* target = FindNode(commit);
	if (target == NULL) return false;

	List_Append(s_RedoStack, s_Here);
	CheckoutFromNode(target, ascended, descended);
	return true;
}

bool UndoTree_Redo(void) {
	if (!s_Enabled || BuildingNode() || List_Count(s_RedoStack) == 0) {
		return false;
	}

	UndoNode* target = List_Pop(s_RedoStack);
	CheckoutFromNode(target, NULL, NULL);
	return true;
}

bool UndoTree_TryPrepareNewNode(char* description) {
	if (!s_Enabled || BuildingNode()) return false;

	s_BuildingNode = malloc(sizeof(UndoNode));

	if (s_BuildingNode == NULL) {
		return false;
	}

	s_CommitAutoIncrement++;
	s_BuildingNode->commit = s_CommitAutoIncrement;
	strncpy(s_BuildingNode->description, description, sizeof(s_BuildingNode->description));
	s_BuildingNode->blocksAffected = 0;
	s_BuildingNode->entries = NULL;
	s_EntriesSize = 0;
	s_BuildingNode->timestamp = time(NULL);
	s_BuildingNode->redoChild = NULL;
	s_BuildingNode->children = List_CreateEmpty();
	return true;
}

void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta) {
	if (!s_Enabled || !BuildingNode()) return;

	BlockChangeEntry entry = {
		.x = x,
		.y = y,
		.z = z,
		.delta = delta
	};

	s_BuildingNode->blocksAffected += 1;

	if (s_BuildingNode->blocksAffected > s_EntriesSize) {
		s_EntriesSize = (s_EntriesSize + 1) * 2;

		BlockChangeEntry* newEntries = realloc(s_BuildingNode->entries, s_EntriesSize * sizeof(BlockChangeEntry));

		if (newEntries == NULL) {
			exit(1);
		}

		s_BuildingNode->entries = newEntries;
	}

	s_BuildingNode->entries[s_BuildingNode->blocksAffected - 1] = entry;
}

void UndoTree_Commit(void) {
	if (!s_Enabled || !BuildingNode()) return;

	if (s_BuildingNode->blocksAffected == 0) {
		FreeUndoNode(s_BuildingNode);
		s_BuildingNode = NULL;
		return;
	}

	Attach(s_Here, s_BuildingNode);
	s_Here = s_BuildingNode;
	List_Append(s_History, s_Here);
	List_Clear(s_RedoStack);
	SetRedoChild(s_Here);
	s_BuildingNode = NULL;
}

void UndoTree_DescribeFiveLastLeaves(cc_string* descriptions, int* descriptionsCount) {
	const int max = 5;
	List* leaves = List_CreateEmpty();
	GetLeaves(leaves);

	*descriptionsCount = List_Count(leaves);

	if (*descriptionsCount > max) {
		*descriptionsCount = max;
	}

	char buffer_formattedTime[STRING_SIZE];
	cc_string formattedTime = String_FromArray(buffer_formattedTime);

	char buffer_formattedBlocks[STRING_SIZE];
	cc_string formattedBlocks = String_FromArray(buffer_formattedBlocks);

	char buffer_formattedCommit[STRING_SIZE];
	cc_string formattedCommit = String_FromArray(buffer_formattedCommit);

	UndoNode* currentNode;

	for (int i = 0; i < *descriptionsCount; i++) {
		currentNode = (UndoNode*) List_Get(leaves, i);

		Format_HHMMSS(&formattedTime, currentNode->timestamp);
		Format_Int32(&formattedBlocks, currentNode->blocksAffected);
		Format_Int32(&formattedCommit, currentNode->commit);

		if (currentNode->blocksAffected == 1) {
			String_Format4(&descriptions[i], "[&b%s&f] %c @ %s (%s block)", &formattedCommit, currentNode->description, &formattedTime, &formattedBlocks);
		} else {
			String_Format4(&descriptions[i], "[&b%s&f] %c @ %s (%s blocks)", &formattedCommit, currentNode->description, &formattedTime, &formattedBlocks);
		}
	}

	List_Free(leaves);
}

long UndoTree_CurrentTimestamp(void) {
	return s_Here->timestamp;
}

static void GetLeaves(List* out_leaves) {
	UndoNode* currentNode;
	List* stack = List_CreateEmpty();
	List_Append(stack, &s_Root);

	while (!List_IsEmpty(stack)) {
		currentNode = (UndoNode*) List_Pop(stack);

		int childrenCount = List_Count(currentNode->children);

		for (int i = 0; i < childrenCount; i++) {
			List_Append(stack, List_Get(currentNode->children, i));
		}

		if (childrenCount == 0) {
			List_Append(out_leaves, currentNode);
		}
	}

	List_Free(stack);
}

static bool TryInitRoot(void) {
	s_Root.commit = 0;
	char description[] = "World loaded";
	strncpy(s_Root.description, description, sizeof(description));
	s_Root.blocksAffected = World.Width * World.Height * World.Length;
	s_Root.entries = NULL;
	s_Root.timestamp = time(NULL);
	s_Root.parent = NULL;
	s_Root.children = List_CreateEmpty();
	s_Root.redoChild = NULL;

	return true;
}

static void FreeUndoNode(UndoNode* node) {
	free(node->entries);
	List_Free(node->children);
	free(node);
}

static bool BuildingNode(void) {
	return s_BuildingNode != NULL;
}

static void Ascend(void) {
	BlockChangeEntry* entries = s_Here->entries;
	BlockID currentBlock; 

	for (int i = 0; i < s_Here->blocksAffected; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock - entries[i].delta);
	}

	s_Here = s_Here->parent;
}

static void Descend(void) {
	s_Here = s_Here->redoChild;
	BlockChangeEntry* entries = s_Here->entries;
	BlockID currentBlock; 

	for (int i = 0; i < s_Here->blocksAffected; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock + entries[i].delta);
	}
}

static UndoNode* FindNode(int commit) {
	UndoNode* currentNode;
	List* stack = List_CreateEmpty();
	List_Append(stack, &s_Root);

	while (!List_IsEmpty(stack)) {
		currentNode = (UndoNode*) List_Pop(stack);

		if (currentNode->commit == commit) {
			List_Free(stack);
			return currentNode;
		}

		int childrenCount = List_Count(currentNode->children);

		for (int i = 0; i < childrenCount; i++) {
			List_Append(stack, List_Get(currentNode->children, i));
		}
	}

	List_Free(stack);
	return NULL;
}

static void Ancestors(UndoNode* node, List* out_ancestors) {
	if (node == NULL) return;

	do {
		List_Append(out_ancestors, node);
		node = node->parent;
	} while (node != NULL);
}

static void CheckoutFromNode(UndoNode* target, int* ascended, int* descended) {
	if (ascended != NULL && descended != NULL) {
		*ascended = 0;
		*descended = 0;
	}

	List* targetAncestors = List_CreateEmpty();
	Ancestors(target, targetAncestors);

	while (!List_Contains(targetAncestors, s_Here)) {
		Ascend();
		if (ascended != NULL) (*ascended)++;
	}

	SetRedoChild(target);

	while (s_Here != target) {
		Descend();
		if (descended != NULL) (*descended)++;
	}

	List_Free(targetAncestors);
}

static void Attach(UndoNode* parent, UndoNode* child) {
	List_Append(parent->children, child);
	child->parent = parent;
}

static void SetRedoChild(UndoNode* node) {
	UndoNode* currentNode = node;

	while (currentNode->parent != NULL) {
		currentNode->parent->redoChild = currentNode;
		currentNode = currentNode->parent;
	}
}

static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block) {
	if (MarkSelection_RemainingMarks() > 0) {
		return;
	}

	char formattedCoordinates[64];
	Format_Coordinates(coords, formattedCoordinates, sizeof(formattedCoordinates));

	char description[64];

	if (block == BLOCK_AIR) {
		snprintf(description, sizeof(description), "Destroy %s", formattedCoordinates);
	} else {
		char formattedBlock[64];
		Format_Block(block, formattedBlock, sizeof(formattedBlock));
		snprintf(description, sizeof(description), "Place %s %s", formattedBlock, formattedCoordinates);
	}
 
	UndoTree_TryPrepareNewNode(description);
	UndoTree_AddBlockChangeEntry(coords.X, coords.Y, coords.Z, block - oldBlock);
	UndoTree_Commit();
}

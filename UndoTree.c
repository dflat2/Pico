#include <stdio.h>
#include <string.h>

#include "CC_API/World.h"
#include "CC_API/Game.h"
#include "CC_API/Chat.h"
#include "CC_API/Event.h"

#include "MemoryAllocation.h"
#include "Messaging.h"
#include "UndoTree.h"
#include "Format.h"
#include "MarkSelection.h"

#include "List.h"

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

static void DescribeNode(UndoNode* node, char* message, size_t length);
static void InitRoot();
static void FreeUndoNode(UndoNode* node);
static bool BuildingNode();
static void Ascend();
static void Descend();
static UndoNode* FindNode(int commit);
static void Ancestors(UndoNode* node, List* out_ancestors);
static void CheckoutFromNode(UndoNode* target, int* ascended, int* descended);
static void Attach(UndoNode* parent, UndoNode* child);
static void SetRedoChild(UndoNode* node);
static void ShowCurrentNode();
static void OnBlockChanged(void* obj, IVec3 coords, BlockID oldBlock, BlockID block);

static UndoNode* s_root = NULL;
static UndoNode* s_here = NULL;
static UndoNode* s_buildingNode = NULL;
static int s_entriesSize = 1;
static int s_commitAutoIncrement = 0;
static bool s_enabled = false;
static List* s_history = NULL;
static List* s_redoStack = NULL;

void UndoTree_Enable() {
	if (s_enabled) return;

	InitRoot();
	s_here = s_root;
	s_buildingNode = NULL;
	s_commitAutoIncrement = 0;
	s_history = List_CreateEmpty();
	List_Append(s_history, s_root);
	s_redoStack = List_CreateEmpty();

	s_enabled = true;
	ShowCurrentNode();

    struct Event_Void* event = (struct Event_Void*) &UserEvents.BlockChanged;
    Event_Void_Callback callback = (Event_Void_Callback)OnBlockChanged;
    Event_Register(event, NULL, callback);
}

void UndoTree_Disable() {
	if (!s_enabled) return;

	UndoNode* currentNode;
	List* stack = List_CreateEmpty();

	List_Append(stack, s_root);

	while (!List_IsEmpty(stack)) {
		currentNode = (UndoNode*) List_Pop(stack);

		int childrenCount = List_Count(currentNode->children);

		for (int i = 0; i < childrenCount; i++) {
			List_Append(stack, List_Get(currentNode->children, i));
		}

		FreeUndoNode(currentNode);
	}

	if (s_buildingNode != NULL) {
		FreeUndoNode(s_buildingNode);
		s_buildingNode = NULL;
	}

	List_Free(stack);
	List_Free(s_history);
	List_Free(s_redoStack);
	s_enabled = false;
	Message_MessageOf("", MSG_TYPE_STATUS_1);
	Message_MessageOf("", MSG_TYPE_STATUS_2);

    struct Event_Void* event = (struct Event_Void*) &UserEvents.BlockChanged;
    Event_Void_Callback callback = (Event_Void_Callback)OnBlockChanged;
    Event_Unregister(event, NULL, callback);
}

bool UndoTree_Enabled() {
	return s_enabled;
}

bool UndoTree_Earlier(int deltaTime_S, int* commit) {
	if (!s_enabled || BuildingNode()) return false;
	if (deltaTime_S <= 0) return false;

	int historyIndex = List_IndexOf(s_history, s_here);

	int newIndex = -1;
	UndoNode* currentNode;

	for (int i = historyIndex - 1; i > 0; i--) {
		currentNode = (UndoNode*) List_Get(s_history, i);
		if (s_here->timestamp - currentNode->timestamp > deltaTime_S) {
			newIndex = i;
			break;
		}
	}

	if (newIndex < 0) {
		newIndex = 0;
	}

	if (newIndex == historyIndex) return false;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target, NULL, NULL);
	ShowCurrentNode();
	*commit = s_here->commit;
	return true;
}

bool UndoTree_Later(int deltaTime_S, int* commit) {
	if (!s_enabled || BuildingNode()) return false;
	if (deltaTime_S <= 0) return false;

	int historyIndex = List_IndexOf(s_history, s_here);

	int count = List_Count(s_history);
	int newIndex = count;
	UndoNode* currentNode;

	for (int i = historyIndex + 1; i < count - 1; i++) {
		currentNode = (UndoNode*) List_Get(s_history, i);
		if (currentNode->timestamp - s_here->timestamp > deltaTime_S) {
			newIndex = i;
			break;
		}
	}

	if (newIndex >= count) {
		newIndex = count - 1;
	}

	if (newIndex == historyIndex) return false;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target, NULL, NULL);
	ShowCurrentNode();
	*commit = s_here->commit;
	return true;
}

bool UndoTree_Ascend() {
	if (!s_enabled || BuildingNode() || s_here->parent == NULL) return false;

	List_Append(s_redoStack, s_here);
	Ascend();
	ShowCurrentNode();
	return true;
}

bool UndoTree_Descend() {
	if (!s_enabled || BuildingNode() || s_here->redoChild == NULL) return false;

	List_Append(s_redoStack, s_here);
	Descend();
	ShowCurrentNode();
	return true;
}

bool UndoTree_Checkout(int commit, int* ascended, int* descended) {
	if (!s_enabled || BuildingNode()) return false;

    UndoNode* target = FindNode(commit);
	if (target == NULL) return false;

	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target, ascended, descended);
	ShowCurrentNode();
	return true;
}

bool UndoTree_Redo() {
	if (!s_enabled || BuildingNode() || List_Count(s_redoStack) == 0) {
		return false;
	}

	UndoNode* target = List_Pop(s_redoStack);
	CheckoutFromNode(target, NULL, NULL);
	ShowCurrentNode();
	return true;
}

void UndoTree_PrepareNewNode(char* description) {
	if (!s_enabled || BuildingNode()) return;

	s_buildingNode = allocate(1, sizeof(UndoNode));
	s_commitAutoIncrement++;
	s_buildingNode->commit = s_commitAutoIncrement;
	strncpy(s_buildingNode->description, description, sizeof(s_buildingNode->description));
	s_buildingNode->blocksAffected = 0;
	s_buildingNode->entries = NULL;
	s_entriesSize = 0;
	s_buildingNode->timestamp = time(NULL);
	s_buildingNode->redoChild = NULL;
	s_buildingNode->children = List_CreateEmpty();
}

void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta) {
	if (!s_enabled || !BuildingNode()) return;

	BlockChangeEntry entry = {
		.x = x,
		.y = y,
		.z = z,
		.delta = delta
	};

	s_buildingNode->blocksAffected += 1;

	if (s_buildingNode->blocksAffected > s_entriesSize) {
		if (s_entriesSize == 0) {
			s_entriesSize = 1;
		} else {
			s_entriesSize *= 2;
		}

		BlockChangeEntry* newEntries = realloc(s_buildingNode->entries, s_entriesSize * sizeof(BlockChangeEntry));

		if (newEntries == NULL) {
			exit(1);
		}

		s_buildingNode->entries = newEntries;
	}

	s_buildingNode->entries[s_buildingNode->blocksAffected - 1] = entry;
}

void UndoTree_Commit() {
	if (!s_enabled || !BuildingNode()) return;

	if (s_buildingNode->blocksAffected == 0) {
		FreeUndoNode(s_buildingNode);
		s_buildingNode = NULL;
		return;
	}

	Attach(s_here, s_buildingNode);
	s_here = s_buildingNode;
	List_Append(s_history, s_here);
	List_Clear(s_redoStack);
	SetRedoChild(s_here);
	s_buildingNode = NULL;
	ShowCurrentNode();
}

static void GetLeaves(List* out_leaves) {
	UndoNode* currentNode;
	List* stack = List_CreateEmpty();
	List_Append(stack, s_root);

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

void UndoTree_ShowLeaves() {
	List* leaves = List_CreateEmpty();
	GetLeaves(leaves);

	int count = List_Count(leaves);
	UndoNode* currentNode;

	for (int i = 0; i < count; i++) {
		currentNode = (UndoNode*) List_Get(leaves, i);

		char formattedTime[] = "00:00:00";
		Format_HHMMSS(currentNode->timestamp, formattedTime, sizeof(formattedTime));

		char formattedBlocks[15];
		Format_Int32(currentNode->blocksAffected, formattedBlocks, sizeof(formattedBlocks));

		char formattedCommit[15];
		Format_Int32(currentNode->commit, formattedCommit, sizeof(formattedCommit));

		char message[64];
		snprintf(message, sizeof(message), "[&e%s&f]&e %s @%s/%s", formattedCommit, currentNode->description, formattedTime, formattedBlocks);

		Message_Player(message);
	}
}

long UndoTree_CurrentTimestamp() {
	return s_here->timestamp;
}

static void InitRoot() {
	s_root = allocate(1, sizeof(UndoNode));
	s_root->commit = 0;
	char description[] = "World loaded";
	strncpy(s_root->description, description, sizeof(description));
	s_root->blocksAffected = World.Width * World.Height * World.Length;
	s_root->entries = NULL;
	s_root->timestamp = time(NULL);
	s_root->parent = NULL;
	s_root->children = List_CreateEmpty();
	s_root->redoChild = NULL;
}

static void FreeUndoNode(UndoNode* node) {
	free(node->entries);
	List_Free(node->children);
	free(node);
}

static bool BuildingNode() {
	return s_buildingNode != NULL;
}

static void Ascend() {
	BlockChangeEntry* entries = s_here->entries;
	BlockID currentBlock; 

	for (int i = 0; i < s_here->blocksAffected; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock - entries[i].delta);
	}

	s_here = s_here->parent;
}

static void Descend() {
	s_here = s_here->redoChild;
	BlockChangeEntry* entries = s_here->entries;
	BlockID currentBlock; 

	for (int i = 0; i < s_here->blocksAffected; i++) {
		currentBlock = World_GetBlock(entries[i].x, entries[i].y, entries[i].z);
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, currentBlock + entries[i].delta);
	}
}

static UndoNode* FindNode(int commit) {
	UndoNode* currentNode;
	List* stack = List_CreateEmpty();
	List_Append(stack, s_root);

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

	while (!List_Contains(targetAncestors, s_here)) {
		Ascend();
		if (ascended != NULL) (*ascended)++;
	}

	SetRedoChild(target);

	while (s_here != target) {
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

static void ShowCurrentNode() {
	char formattedTime[] = "00:00:00";
	Format_HHMMSS(s_here->timestamp, formattedTime, sizeof(formattedTime));

	char formattedBlocks[15];
	Format_Int32(s_here->blocksAffected, formattedBlocks, sizeof(formattedBlocks));

	char formattedCommit[15];
	Format_Int32(s_here->commit, formattedCommit, sizeof(formattedCommit));

	char status1[64];
	snprintf(status1, sizeof(status1), "[&e%s&f]&e %s @%s", formattedCommit, s_here->description, formattedTime);

	char status2[64];
	snprintf(status2, sizeof(status2), "Blocks affected: &e%s", formattedBlocks);

	Message_MessageOf(status1, MSG_TYPE_STATUS_1);
	Message_MessageOf(status2, MSG_TYPE_STATUS_2);
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
 
	UndoTree_PrepareNewNode(description);
	UndoTree_AddBlockChangeEntry(coords.X, coords.Y, coords.Z, block - oldBlock);
	UndoTree_Commit();
}

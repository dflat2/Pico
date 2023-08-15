#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "CC_API/World.h"
#include "CC_API/Block.h"
#include "CC_API/Game.h"

#include "MemoryAllocation.h"
#include "Messaging.h"
#include "UndoTree.h"

#include "List.h"
#include "TimeFunctions.h"

typedef struct BlockChangeEntry_ {
	signed short x;
	signed short y;
	signed short z;
	DeltaBlockID delta;
} BlockChangeEntry;

typedef struct UndoNode_ UndoNode;

typedef struct UndoNode_ {
	int commit;
	char description[64];
	int blocksAffected;
	BlockChangeEntry* entries;
	long timestamp_Millisecond;
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
static void CheckoutFromNode(UndoNode* target);
static void Attach(UndoNode* parent, UndoNode* child);
static void SetRedoChild(UndoNode* node);
static void DescribeNode(UndoNode* node, char* message, size_t length);

static UndoNode* s_root = NULL;
static UndoNode* s_here = NULL;
static UndoNode* s_buildingNode = NULL;
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
}

bool UndoTree_Enabled() {
	return s_enabled;
}

void UndoTree_Earlier_Second(int deltaTime_Second) {
	if (!s_enabled || BuildingNode()) return;
	if (deltaTime_Second <= 0) return;

	int historyIndex = List_IndexOf(s_history, s_here);
	long deltaTime_Millisecond = deltaTime_Second * 1000;
	long here_Millisecond = s_here->timestamp_Millisecond;

	int newIndex = -1;
	UndoNode* currentNode;

	for (int i = historyIndex - 1; i > 0; i--) {
		currentNode = (UndoNode*) List_Get(s_history, i);
		if (here_Millisecond - currentNode->timestamp_Millisecond > deltaTime_Millisecond) {
			newIndex = i;
			break;
		}
	}

	if (newIndex < 0) {
		newIndex = 0;
	}

	if (newIndex == historyIndex) return;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target);
}

void UndoTree_Later_Seconds(int deltaTime_Second) {
	if (!s_enabled || BuildingNode()) return;
	if (deltaTime_Second <= 0) return;

	int historyIndex = List_IndexOf(s_history, s_here);
	long deltaTime_Millisecond = deltaTime_Second * 1000;
	long here_Millisecond = s_here->timestamp_Millisecond;

	int count = List_Count(s_history);
	int newIndex = count;
	UndoNode* currentNode;

	for (int i = historyIndex + 1; i < count - 1; i++) {
		currentNode = (UndoNode*) List_Get(s_history, i);
		if (currentNode->timestamp_Millisecond - here_Millisecond > deltaTime_Millisecond) {
			newIndex = i;
			break;
		}
	}

	if (newIndex >= count) {
		newIndex = count - 1;
	}

	if (newIndex == historyIndex) return;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target);
}

void UndoTree_Earlier(int count) {
	if (!s_enabled || BuildingNode()) return;
	if (count <= 0) return;

	int historyIndex = List_IndexOf(s_history, s_here);
	int newIndex = historyIndex - count;

	if (newIndex < 0) {
		newIndex = 0;
	}

	if (newIndex == historyIndex) return;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target);
}

void UndoTree_Later(int count) {
	if (!s_enabled || BuildingNode()) return;
	if (count <= 0) return;

	int historyIndex = List_IndexOf(s_history, s_here);
	int newIndex = historyIndex + count;

	if (newIndex >= List_Count(s_history)) {
		newIndex = List_Count(s_history) - 1;
	}

	if (newIndex == historyIndex) return;

	UndoNode* target = List_Get(s_history, newIndex);
	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target);
}

int UndoTree_Ascend(int amount) {
	if (!s_enabled || BuildingNode()) return 0;
	int amountAscended = 0;
	List_Append(s_redoStack, s_here);

	for (int i = 0; i < amount; i++) {
		if (s_here->parent == NULL) {
			if (amountAscended == 0) {
				List_Pop(s_redoStack);
			}
			return amountAscended;
		}

		Ascend();
		amountAscended++;
	}

	return amountAscended;
}

int UndoTree_Descend(int amount) {
	if (!s_enabled || BuildingNode()) return 0;
	int amountDescended = 0;
	List_Append(s_redoStack, s_here);

	for (int i = 0; i < amount; i++) {
		if (s_here->redoChild == NULL) {
			if (amountDescended == 0) {
				List_Pop(s_redoStack);
			}
			return amountDescended;
		}

		Descend();
		amountDescended++;
	}

    return amountDescended;
}

void UndoTree_Checkout(int commit) {
	if (!s_enabled || BuildingNode()) return;

    UndoNode* target = FindNode(commit);
	if (target == NULL) return;

	List_Append(s_redoStack, s_here);
	CheckoutFromNode(target);
}

void UndoTree_Redo(int count) {
	if (!s_enabled || BuildingNode()) return;
	if (count <= 0) return;

	UndoNode* lastPop = NULL;

	for (int i = 0; i < count; i++) {
		if (List_Count(s_redoStack) == 0) break;
		List_Pop(s_redoStack);
	}

	if (lastPop != NULL) {
		CheckoutFromNode(lastPop);
	}
}

void UndoTree_PrepareNewNode(char* description) {
	if (!s_enabled || BuildingNode()) return;

	s_buildingNode = allocate(1, sizeof(UndoNode));
	s_commitAutoIncrement++;
	s_buildingNode->commit = s_commitAutoIncrement;
	strncpy(s_buildingNode->description, description, sizeof(s_buildingNode->description));
	s_buildingNode->blocksAffected = 0;
	s_buildingNode->entries = NULL;
	s_buildingNode->timestamp_Millisecond = Time_Now_Millisecond();
	s_buildingNode->redoChild = NULL;
	s_buildingNode->children = List_CreateEmpty();
}

void UndoTree_AddBlockChangeEntry(signed short x, signed short y, signed short z, DeltaBlockID delta) {
	if (!s_enabled || !BuildingNode()) return;

	BlockChangeEntry entry = {
		.x = x,
		.y = y,
		.z = z,
		.delta = delta
	};

	s_buildingNode->blocksAffected += 1;
	BlockChangeEntry* newEntries = realloc(s_buildingNode->entries, s_buildingNode->blocksAffected * sizeof(BlockChangeEntry));

	if (newEntries == NULL) {
		exit(1);
	}

	s_buildingNode->entries = newEntries;
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
}

void UndoTree_ShowLeaves() {
	if (!s_enabled) {
		return;
	}

	char* message = malloc(128 * sizeof(char));

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
			DescribeNode(currentNode, message, 128);
			PlayerMessage(message);
		}
	}

	List_Free(stack);
}

static void DescribeNode(UndoNode* node, char* message, size_t length) {
    int commit = node->commit;
    char* description = node->description;
    int blocksAffected = node->blocksAffected;

	char formattedTime[] = "00:00";
	Time_FormatDayTime(formattedTime, sizeof(formattedTime), Time_UnixTimeToDayTime(node->timestamp_Millisecond));

    char plural[] = "s";
    if (blocksAffected != 1) plural[0] = '\0';

    snprintf(message, length, "%d. %s (%d block%s) [%s]", commit, description, blocksAffected, plural, formattedTime);
}

static void InitRoot() {
	s_root = allocate(1, sizeof(UndoNode));
	s_root->commit = 0;
	char description[] = "World loaded";
	strncpy(s_root->description, description, sizeof(description));
	s_root->blocksAffected = World.Width * World.Height * World.Length;
	s_root->entries = NULL;
	s_root->timestamp_Millisecond = Time_Now_Millisecond();
	s_root->parent = NULL;
	s_root->children = List_CreateEmpty();
	s_root->redoChild = NULL;
}

static void FreeUndoNode(UndoNode* node) {
	free(node->entries);
	free(node->parent);
	List_Free(node->children);
	free(node);
}

static bool BuildingNode() {
	return s_buildingNode != NULL;
}

static void Ascend() {
	BlockChangeEntry* entries = s_here->entries;

	for (int i = 0; i < s_here->blocksAffected; i++) {
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, World_GetRawBlock(i) - entries[i].delta);
	}

	s_here = s_here->parent;
}

static void Descend() {
	s_here = s_here->redoChild;
	BlockChangeEntry* entries = s_here->entries;

	for (int i = 0; i < s_here->blocksAffected; i++) {
		Game_UpdateBlock(entries[i].x, entries[i].y, entries[i].z, World_GetRawBlock(i) + entries[i].delta);
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
	} while (node->parent != NULL);
}

static void CheckoutFromNode(UndoNode* target) {
	List* targetAncestors = List_CreateEmpty();

	while (!List_Contains(targetAncestors, s_here)) {
		Ascend();
	}

	SetRedoChild(target);

	while (s_here != target) {
		Descend();
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

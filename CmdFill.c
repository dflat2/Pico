#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "ParsingUtils.h"
#include "Array.h"
#include "SPCCommand.h"
#include "List.h"
#include "WorldUtils.h"

typedef enum FillMode_ {
	MODE_3D = 0,
	MODE_2DX = 1,
	MODE_2DY = 2,
	MODE_2DZ = 3,
} FillMode;

typedef struct IVec3QueueNode_ {
	struct IVec3QueueNode_* next;
	IVec3 content;
} IVec3QueueNode;

typedef struct IVec3Queue_ {
	IVec3QueueNode* first;
} IVec3Queue;

static FillMode s_Mode;

static void Fill_Command(const cc_string* args, int argsCount);
static bool TryParseArguments(const cc_string* args, int argsCount);
static void ShowUsage();
static void FillSelectionHandler(IVec3* marks, int count);
static IVec3Queue* IVec3Queue_CreateEmpty();
static bool IVec3Queue_IsEmpty(IVec3Queue* queue);
static void IVec3Queue_Free(IVec3Queue* queue);
static void IVec3Queue_Enqueue(IVec3Queue* queue, IVec3 vector);
static IVec3 IVec3Queue_Dequeue(IVec3Queue* queue);
static bool IVec3Queue_Contains(IVec3Queue* queue, IVec3 vector);

static struct ChatCommand FillCommand = {
	"Fill",
	Fill_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Fill [mode] [brush/block]",
        "&fFills the specified area.",
		"&fList of modes: &b3d&f (default), &b2d-x&f, &b2d-y&f, &b2d-z&f.",
		NULL,
		NULL
	},
	NULL
};

SPCCommand FillSPCCommand = {
	.chatCommand = &FillCommand,
	.canStatic = true
};

static void ShowUsage() {
	Message_Player("Usage: &b/Fill [mode] [brush/block]&f.");
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("3d"),
        String_FromConst("2d-x"),
        String_FromConst("2d-y"),
        String_FromConst("2d-z"),
    };

	size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

	bool hasBlockOrBrush = (argsCount >= 2) ||
		((argsCount == 1) && (!Array_ContainsString(&args[0], modesString, modesCount)));
	bool hasMode = (argsCount >= 1) && Array_ContainsString(&args[0], modesString, modesCount);

	if (hasMode) {
		s_Mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
	} else {
		s_Mode = MODE_3D;
	}

	if (hasMode && s_Mode == -1) {
		Message_ShowUnknownMode(&args[0]);
		Message_ShowAvailableModes(modesString, modesCount);
		return false;
	}

	if (hasBlockOrBrush) {
		int brushIndex;

		if (hasMode) {
			brushIndex = 1;
		} else {
			brushIndex = 0;
		}

		// Checks that there are no trailing blocks in the command, e.g. `/Z Stone Air` doesn't make sense.
		bool isBlock = args[brushIndex].buffer[0] != '@';
		if (isBlock && argsCount > (brushIndex + 1)) {
			ShowUsage();
			return false;
		}

		if (!Parse_TryParseBlockOrBrush(&args[brushIndex], argsCount - brushIndex)) {
			return false;
		}

		return true;
	} else {
		Brush_LoadInventory();
		return true;
	}
}

void EnqueueNonVisitedNeighbors(IVec3Queue* queue, IVec3 target, BlockID filledOverBlock, IVec3Queue* visited) {
	IVec3 allNeighbors[6] = {
		{ target.X - 1, target.Y, target.Z },
		{ target.X + 1, target.Y, target.Z },
		{ target.X, target.Y - 1, target.Z },
		{ target.X, target.Y + 1, target.Z },
		{ target.X, target.Y, target.Z - 1 },
		{ target.X, target.Y, target.Z + 1 },
	};

	IVec3 neighbor;

	for (int i = 0; i < 6; i++) {
		neighbor = allNeighbors[i];

		if (!IsInWorldBoundaries(neighbor.X, neighbor.Y, neighbor.Z)) {
			continue;
		}

		if (GetBlock(neighbor.X, neighbor.Y, neighbor.Z) != filledOverBlock) {
			continue;
		}

		if (IVec3Queue_Contains(visited, neighbor)) {
			continue;
		}

		IVec3Queue_Enqueue(queue, neighbor);
		IVec3Queue_Enqueue(visited, neighbor);
	}
}

static void FillSelectionHandler(IVec3* marks, int count) {
	IVec3 fillOrigin = marks[0];
	BlockID filledOverBlock = GetBlock(fillOrigin.X, fillOrigin.Y, fillOrigin.Z);
	IVec3Queue* queue = IVec3Queue_CreateEmpty();
	IVec3Queue* visited = IVec3Queue_CreateEmpty();

	IVec3Queue_Enqueue(queue, fillOrigin);
	IVec3Queue_Enqueue(visited, fillOrigin);
	IVec3 current;

	while (!IVec3Queue_IsEmpty(queue)) {
		current = IVec3Queue_Dequeue(queue);
		EnqueueNonVisitedNeighbors(queue, current, filledOverBlock, visited);
	}

	Draw_Start("Fill");

	while (!IVec3Queue_IsEmpty(visited)) {
		current = IVec3Queue_Dequeue(visited);
		Draw_Brush(current.X, current.Y, current.Z);
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);

	IVec3Queue_Free(visited);
	IVec3Queue_Free(queue);
}

static void Fill_Command(const cc_string* args, int argsCount) {
	if (!TryParseArguments(args, argsCount)) {
		MarkSelection_Abort();
		return;
	}

    MarkSelection_Make(FillSelectionHandler, 1);
    Message_Player("&fPlace or break a block.");
}

static IVec3Queue* IVec3Queue_CreateEmpty() {
	IVec3Queue* queue = malloc(sizeof(IVec3Queue));

	if (queue == NULL) {
		return NULL;
	}

	queue->first = NULL;
	return queue;
}

static bool IVec3Queue_IsEmpty(IVec3Queue* queue) {
	return queue->first == NULL;
}

static void IVec3Queue_Free(IVec3Queue* queue) {
	while (!IVec3Queue_IsEmpty(queue)) {
		IVec3Queue_Dequeue(queue);
	}

	free(queue);
}

static void IVec3Queue_Enqueue(IVec3Queue* queue, IVec3 vector) {
	IVec3QueueNode* newElement = malloc(sizeof(IVec3QueueNode));
	newElement->content = vector;
	newElement->next = NULL;

	if (IVec3Queue_IsEmpty(queue)) {
		queue->first = newElement;
		return;
	}

	IVec3QueueNode* current = queue->first;

	while (current->next != NULL) {
		current = current->next;
	}

	current->next = newElement;
}

static IVec3 IVec3Queue_Dequeue(IVec3Queue* queue) {
	IVec3QueueNode* first = queue->first;
	IVec3 result = first->content;
	queue->first = first->next;
	free(first);
	return result;
}

static bool IVec3Queue_Contains(IVec3Queue* queue, IVec3 vector) {
	IVec3QueueNode* current = queue->first;

	while (current != NULL) {
		if (current->content.X == vector.X && current->content.Y == vector.Y && current->content.Z == vector.Z) {
			return true;
		}

		current = current->next;
	}

	return false;
}

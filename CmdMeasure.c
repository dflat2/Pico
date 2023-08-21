#include <stdio.h>

#include "CC_API/Block.h"
#include "CC_API/Chat.h"

#include "Messaging.h"
#include "MemoryAllocation.h"
#include "WorldUtils.h"
#include "MarkSelection.h"
#include "Vectors.h"
#include "SPCCommand.h"

static void Measure_Command(const cc_string* args, int argsCount);
static void CleanResources(void* arguments);
static void MeasureSelectionHandler(IVec3* marks, int count, void* object);
static void CountBlocks(int x1, int y1, int z1, int x2, int y2, int z2, BlockID* blocks, int count);
static void ShowCountedBlocks(int* counts, BlockID* blocks, int count);

static struct ChatCommand MeasureCommand = {
	"Measure",
	Measure_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Measure",
		"&fDisplay the dimensions between two points.",
		"&b/Measure <block>",
		"&fAdditionally, counts the number of &b<block>&fs.",
		NULL
	},
	NULL
};

SPCCommand MeasureSPCCommand = {
	.chatCommand = &MeasureCommand,
	.canStatic = true
};


typedef struct MeasureExtraArguments_ {
	BlockID* blocks;
	unsigned count;
} MeasureExtraArguments;

static void ShowCountedBlocks(int* counts, BlockID* blocks, int count) {
	cc_string currentBlockName;
	char buffer[128];
	cc_string currentMessage = { buffer, 0, 128 };

	for (int i = 0; i < count; i++) {
		currentBlockName = Block_UNSAFE_GetName(blocks[i]);
		currentMessage.length = 0;
		String_Format2(&currentMessage, "&b%s&f: &b%i", &currentBlockName, &counts[i]);
		Chat_Add(&currentMessage);
	}
}

static void CountBlocks(int x1, int y1, int z1, int x2, int y2, int z2, BlockID* blocks, int count) {
	int* counts = allocateZeros(count, sizeof(int));
	BlockID currentBlock;

	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			for (int z = z1; z <= z2; z++) {
				currentBlock = GetBlock(x, y, z);

				for (int i = 0; i < count; i++) {
					if (blocks[i] == currentBlock) {
						counts[i]++;
						break;
					}
				}
			}
		}
	}

	ShowCountedBlocks(counts, blocks, count);
	free(counts);
}

static void MeasureSelectionHandler(IVec3* marks, int count, void* object) {
	if (count != 2) {
		return;
	}

	char message[64];
	snprintf(&message[0], 64, "&fMeasuring from &b(%d, %d, %d)&f to &b(%d, %d, %d)&f.",
	        marks[0].X, marks[0].Y, marks[0].Z,
			marks[1].X, marks[1].Y, marks[1].Z);
	Message_Player(&message[0]);

	int width = abs(marks[0].X - marks[1].X) + 1;
	int height = abs(marks[0].Y - marks[1].Y) + 1;
	int length = abs(marks[0].Z - marks[1].Z) + 1;
	int volume = width * height * length;

	snprintf(&message[0], 64, "&b%d &fwide, &b%d &fhigh, &b%d &flong, &b%d &fblocks.", width, height, length, volume);
	Message_Player(&message[0]);

	if (object == NULL) {
		return;
	}

	IVec3 min = Min(marks[0], marks[1]);
	IVec3 max = Max(marks[0], marks[1]);
	
	MeasureExtraArguments* arguments = (MeasureExtraArguments*) object;
	CountBlocks(min.X, min.Y, min.Z, max.X, max.Y, max.Z,
				arguments->blocks, arguments->count);
}

static void CleanResources(void* arguments) {
	MeasureExtraArguments* measureExtraArguments = arguments;
	free(measureExtraArguments->blocks);
	free(measureExtraArguments);
}

static void Measure_Command(const cc_string* args, int argsCount) {
	if (argsCount == 0) {
		MarkSelection_Make(MeasureSelectionHandler, 2, NULL, NULL);
	}

	BlockID* blocks = allocate(argsCount, sizeof(blocks));
	int currentBlock;

	for (int i = 0; i < argsCount; i++) {
		currentBlock = Block_Parse(&args[i]);

		if (currentBlock == -1) {
			char message[64];
			cc_string ccMessage = { message, 0, 64 };
			String_Format1(&ccMessage, "&fThere is no block &b%s&f.", (void*)&args[i]);
			Chat_Add(&ccMessage);
			free(blocks);
			return;
		}

		blocks[i] = (BlockID)currentBlock;
	}

	MeasureExtraArguments* arguments = allocate(1, sizeof(MeasureExtraArguments));
	arguments->blocks = blocks;
	arguments->count = argsCount;

	MarkSelection_Make(MeasureSelectionHandler, 2, arguments, CleanResources);
	Message_Player("&fPlace or break two blocks to determine the edges.");
}

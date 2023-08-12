#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Core.h"
#include "CC_API/Game.h"
#include "CC_API/Inventory.h"
#include "CC_API/Vectors.h"

#include "BlocksBuffer.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "ParsingUtils.h"
#include "Vectors.h"
#include "WorldUtils.h"

static void ShowBlocksCopied(int amount) {
	char message[128];

	if (amount == 1) {
		snprintf(message, sizeof(message), "&b%d &fblock were copied.", amount);
	} else {
		snprintf(message, sizeof(message), "&b%d &fblocks were copied.", amount);
	}

	PlayerMessage(message);
}

static void CopySelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }

	IVec3 min = Min(marks[0], marks[1]);
	IVec3 max = Max(marks[0], marks[1]);
	IVec3 anchor = Substract(marks[0], min);

	int width = max.X - min.X + 1;
	int height = max.Y - min.Y + 1;
	int length = max.Z - min.Z + 1;

	BlockID* blocks = allocateZeros(width * height * length, sizeof(BlockID));
	int index = 0;

	for (int x = min.X; x <= max.X; x++) {
		for (int y = min.Y; y <= max.Y; y++) {
			for (int z = min.Z; z <= max.Z; z++) {
				blocks[index] = GetBlock(x, y, z);
				index++;
			}
		}
	}

	BlocksBuffer buffer = {
		.width = max.X - min.X + 1,
		.height = max.Y - min.Y + 1,
		.length = max.Z - min.Z + 1,
		.content = blocks,
		.anchor = anchor
	};

	SetCopiedBuffer(buffer);
	ShowBlocksCopied(buffer.width * buffer.height * buffer.length);
}

static void Copy_Command(const cc_string* args, int argsCount) {
    MakeSelection(CopySelectionHandler, 2, NULL, NULL);
    PlayerMessage("&fPlace or break two blocks to determine the edges.");
}

struct ChatCommand CopyCommand = {
	"Copy",
	Copy_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Copy &f- Copies the blocks in an area.",
		"&b/Copy save <name> &f- Saves what you have copied.",
        "&b/Copy load <name> &f- Loads what you have saved.",
		"&b/Copy list &f- Lists what you have saved.",
		"&b/Copy delete <name> &f- Deletes the specified copy."
	},
	NULL
};

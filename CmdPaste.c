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

typedef enum PasteMode_ {
    NORMAL,
    AIR,
} PasteMode;

typedef struct PasteArgs_ {
    PasteMode mode;
} PasteArgs;


static void ShowBlocksPasted(int amount) {
	char message[128];

	if (amount == 1) {
		snprintf(message, sizeof(message), "&b%d &fblock were pasted.", amount);
	} else {
		snprintf(message, sizeof(message), "&b%d &fblocks were pasted.", amount);
	}

	PlayerMessage(message);
}


static void PasteSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 1 || BufferIsEmpty()) {
        return;
    }

	BlocksBuffer buffer = GetCopiedBuffer();
	IVec3 origin = Substract(marks[0], buffer.anchor);
	int index = -1;

	for (int x = origin.X; x < origin.X + buffer.width; x++) {
		for (int y = origin.Y; y < origin.Y + buffer.height; y++) {
			for (int z = origin.Z; z < origin.Z + buffer.length; z++) {
				index++;
				if (!IsInWorldBoundaries(x, y, z)) continue;
				Game_UpdateBlock(x, y, z, buffer.content[index]);
			}
		}
	}

	ShowBlocksPasted(buffer.width * buffer.height * buffer.length);
}

static void CleanResources(void* args) {
	PasteArgs* pasteArgs = (PasteArgs*)args;
	free(pasteArgs);
}

static void Paste_Command(const cc_string* args, int argsCount) {
	if (BufferIsEmpty()) {
		PlayerMessage("&fYou haven't copied anything yet.");
		return;
	}

	PasteArgs* pasteArgs = allocateZeros(1, sizeof(pasteArgs));

    MakeSelection(PasteSelectionHandler, 1, pasteArgs, CleanResources);
    PlayerMessage("&fPlace a block in the corner of where you want to paste.");
}

struct ChatCommand PasteCommand = {
	"Paste",
	Paste_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Paste [mode] &f- Pastes the stored copy.",
		"&fSet the mode to &b:air &fto also paste air blocks.",
		"&fList of modes: &b:normal&f (default), &b:air&f.",
		NULL,
		NULL
	},
	NULL
};

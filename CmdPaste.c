#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/BlockID.h"
#include "CC_API/Chat.h"
#include "CC_API/Core.h"
#include "CC_API/Game.h"
#include "CC_API/Inventory.h"
#include "CC_API/Vectors.h"

#include "Draw.h"
#include "BlocksBuffer.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "ParsingUtils.h"
#include "Vectors.h"
#include "WorldUtils.h"
#include "Array.h"

typedef enum PasteMode_ {
    NORMAL,
    AIR,
} PasteMode;

typedef struct PasteArguments_ {
    PasteMode mode;
} PasteArguments;


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

	Draw_Start("Paste");
	PasteArguments* pasteArgs = (PasteArguments*)object;
	BlocksBuffer buffer = GetCopiedBuffer();
	IVec3 origin = Substract(marks[0], buffer.anchor);
	int index = -1;

	for (int x = origin.X; x < origin.X + buffer.width; x++) {
		for (int y = origin.Y; y < origin.Y + buffer.height; y++) {
			for (int z = origin.Z; z < origin.Z + buffer.length; z++) {
				index++;
				if (!IsInWorldBoundaries(x, y, z)) continue;

				if (pasteArgs->mode == AIR || buffer.content[index] != BLOCK_AIR) {
					Draw_Block(x, y, z, buffer.content[index]);
				}
			}
		}
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
	ShowBlocksPasted(buffer.width * buffer.height * buffer.length);
}

static void CleanResources(void* args) {
	PasteArguments* pasteArgs = (PasteArguments*)args;
	free(pasteArgs);
}

static void Paste_Command(const cc_string* args, int argsCount) {
	if (BufferIsEmpty()) {
		PlayerMessage("&fYou haven't copied anything yet.");
		return;
	}

	if (argsCount >= 2) {
		PlayerMessage("Usage: &b/Paste [mode]&f.");
		return;
	}

	PasteArguments* pasteArgs = allocateZeros(1, sizeof(PasteArguments));

	if (argsCount == 0) {
		pasteArgs->mode = NORMAL;
	} else if (argsCount == 1) {
		cc_string modesString[] = {
			String_FromConst("normal"),
			String_FromConst("air"),
		};

		int modesCount = 2;
		int modeIndex = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);

		if (modeIndex == -1) {
			Message_ShowUnknownMode(&args[0]);
			Message_ShowAvailableModes(modesString, modesCount);
			free(pasteArgs);
			return;
		}

		pasteArgs->mode = (PasteMode)modeIndex;
	}

    MakeSelection(PasteSelectionHandler, 1, pasteArgs, CleanResources);
    PlayerMessage("&fPlace a block in the corner of where you want to paste.");
}

struct ChatCommand PasteCommand = {
	"Paste",
	Paste_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Paste [mode] &f- Pastes the stored copy.",
		"&fSet the mode to &bair &fto also paste air blocks.",
		"&fList of modes: &bnormal&f (default), &bair&f.",
		NULL,
		NULL
	},
	NULL
};

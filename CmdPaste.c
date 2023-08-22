#include <stdio.h>

#include "CC_API/Chat.h"

#include "Draw.h"
#include "BlocksBuffer.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "WorldUtils.h"
#include "Array.h"
#include "SPCCommand.h"

static void Paste_Command(const cc_string* args, int argsCount);
static void CleanResources(void* args);
static void PasteSelectionHandler(IVec3* marks, int count, void* object);
static void ShowBlocksPasted(int amount);

static struct ChatCommand PasteCommand = {
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

SPCCommand PasteSPCCommand = {
	.chatCommand = &PasteCommand,
	.canStatic = true
};


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

	Message_Player(message);
}


static void PasteSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 1 || BlocksBuffer_IsEmpty()) {
        return;
    }

	Draw_Start("Paste");
	PasteArguments* pasteArgs = (PasteArguments*)object;
	BlocksBuffer buffer = BlocksBuffer_GetCopied();
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
	if (BlocksBuffer_IsEmpty()) {
		Message_Player("&fYou haven't copied anything yet.");
		MarkSelection_Abort();
		return;
	}

	if (argsCount >= 2) {
		Message_Player("Usage: &b/Paste [mode]&f.");
		MarkSelection_Abort();
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
			MarkSelection_Abort();
			return;
		}

		pasteArgs->mode = (PasteMode)modeIndex;
	}

    MarkSelection_Make(PasteSelectionHandler, 1, pasteArgs, CleanResources);
    Message_Player("&fPlace a block in the corner of where you want to paste.");
}

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
static void PasteSelectionHandler(IVec3* marks, int count);
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
    MODE_NORMAL,
    MODE_AIR,
} PasteMode;

static PasteMode s_Mode;

static void ShowBlocksPasted(int amount) {
	char message[128];

	if (amount == 1) {
		snprintf(message, sizeof(message), "&b%d &fblock were pasted.", amount);
	} else {
		snprintf(message, sizeof(message), "&b%d &fblocks were pasted.", amount);
	}

	Message_Player(message);
}


static void PasteSelectionHandler(IVec3* marks, int count) {
    if (count != 1 || BlocksBuffer_IsEmpty()) {
        return;
    }

	Draw_Start("Paste");
	BlocksBuffer buffer = BlocksBuffer_GetCopied();
	IVec3 origin = Substract(marks[0], buffer.anchor);
	int index = -1;

	for (int x = origin.X; x < origin.X + buffer.width; x++) {
		for (int y = origin.Y; y < origin.Y + buffer.height; y++) {
			for (int z = origin.Z; z < origin.Z + buffer.length; z++) {
				index++;
				if (!IsInWorldBoundaries(x, y, z)) continue;

				if (s_Mode == MODE_AIR || buffer.content[index] != BLOCK_AIR) {
					Draw_Block(x, y, z, buffer.content[index]);
				}
			}
		}
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
	ShowBlocksPasted(buffer.width * buffer.height * buffer.length);
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

	if (argsCount == 0) {
		s_Mode = MODE_NORMAL;
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
			MarkSelection_Abort();
			return;
		}

		s_Mode = (PasteMode)modeIndex;
	}

    MarkSelection_Make(PasteSelectionHandler, 1);
    Message_Player("&fPlace a block in the corner of where you want to paste.");
}

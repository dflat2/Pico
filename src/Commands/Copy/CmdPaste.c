#include <stdio.h>

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/World.h"

#include "Draw.h"
#include "DataStructures/BlocksBuffer.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "DataStructures/Array.h"

static void Paste_Command(const cc_string* args, int argsCount);
static void PasteSelectionHandler(IVec3* marks, int count);
static void ShowBlocksPasted(int amount);

struct ChatCommand PasteCommand = {
	"Paste",
	Paste_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Paste [mode]",
		"Pastes the stored copy.",
		"&fList of modes: &bnormal&f (default), &bair&f.",
		NULL,
		NULL,
	},
	NULL
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

	for (int x = origin.X; x < origin.X + buffer.dimensions.X; x++) {
		for (int y = origin.Y; y < origin.Y + buffer.dimensions.Y; y++) {
			for (int z = origin.Z; z < origin.Z + buffer.dimensions.Z; z++) {
				index++;
				if (!World_Contains(x, y, z)) continue;

				if (s_Mode == MODE_AIR || buffer.content[index] != BLOCK_AIR) {
					Draw_Block(x, y, z, buffer.content[index]);
				}
			}
		}
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
	ShowBlocksPasted(buffer.dimensions.X * buffer.dimensions.Y * buffer.dimensions.Z);
}

static void Paste_Command(const cc_string* args, int argsCount) {
	if (BlocksBuffer_IsEmpty()) {
		Message_Player("&fYou haven't copied anything yet.");
		MarkSelection_Abort();
		return;
	}

	if (argsCount >= 2) {
		Message_CommandUsage(PasteCommand);
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

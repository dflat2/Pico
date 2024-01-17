#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "DataStructures/BlocksBuffer.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "SPCCommand.h"

static void CutSelectionHandler(IVec3* marks, int count);
static void Cut_Command(const cc_string* args, int argsCount);
static void DoCut(IVec3 mark1, IVec3 mark2);
static void ShowBlocksCut(int amount);

static struct ChatCommand CutCommand = {
	"Cut",
	Cut_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Cut &f- Copies and cut the blocks in an area.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand CutSPCCommand = {
	.chatCommand = &CutCommand,
	.canStatic = false
};


static void ShowBlocksCut(int amount) {
	char message[128];

	if (amount == 1) {
		snprintf(message, sizeof(message), "&b%d &fblock were cut.", amount);
	} else {
		snprintf(message, sizeof(message), "&b%d &fblocks were cut.", amount);
	}

	Message_Player(message);
}

static void DoCut(IVec3 mark1, IVec3 mark2) {
	IVec3 min = Min(mark1, mark2);
	IVec3 max = Max(mark1, mark2);

	Draw_Start("Cut");
	for (int x = min.X; x <= max.X; x++) {
		for (int y = min.Y; y <= max.Y; y++) {
			for (int z = min.Z; z <= max.Z; z++) {
				Draw_Block(x, y, z, BLOCK_AIR);
			}
		}
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void CutSelectionHandler(IVec3* marks, int count) {
    if (count != 2) {
        return;
    }

	int amountCopied = 0;

	if (!BlocksBuffer_TryCopy(marks[0], marks[1], &amountCopied)) {
		Message_Player("Error when doing the cut.");
		return;
	}
	DoCut(marks[0], marks[1]);
	ShowBlocksCut(amountCopied);
}

static void Cut_Command(const cc_string* args, int argsCount) {
    MarkSelection_Make(CutSelectionHandler, 2);
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

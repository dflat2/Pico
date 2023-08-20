#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"
#include "CC_API/Vectors.h"

#include "BlocksBuffer.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "Vectors.h"

static void ShowBlocksCut(int amount) {
	char message[128];

	if (amount == 1) {
		snprintf(message, sizeof(message), "&b%d &fblock were cut.", amount);
	} else {
		snprintf(message, sizeof(message), "&b%d &fblocks were cut.", amount);
	}

	PlayerMessage(message);
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

static void CutSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }
	int amountCut = Copy(marks[0], marks[1]);
	DoCut(marks[0], marks[1]);
	ShowBlocksCut(amountCut);
}

static void Cut_Command(const cc_string* args, int argsCount) {
    MakeSelection(CutSelectionHandler, 2, NULL, NULL);
    PlayerMessage("&fPlace or break two blocks to determine the edges.");
}

struct ChatCommand CutCommand = {
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

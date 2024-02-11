#include <stdio.h>

#include "DataStructures/BlocksBuffer.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorsExtension.h"
#include "Memory.h"

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
    BlocksBuffer_Copy_MALLOC(marks[0], marks[1], &amountCopied);

    if (Memory_AllocationError()) {
        Memory_HandleError();
        Message_MemoryError("running &b/Copy");
        return;
    }

    DoCut(marks[0], marks[1]);
    ShowBlocksCut(amountCopied);
}

static void Cut_Command(const cc_string* args, int argsCount) {
    MarkSelection_Make(CutSelectionHandler, 2, "Cut");
    Message_Player("Place or break two blocks to determine the edges.");
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

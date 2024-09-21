#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Chat.h"

#include "DataStructures/BlocksBuffer.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Format.h"
#include "VectorUtils.h"
#include "Memory.h"

static void ShowBlocksCut(int amount) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    char blocksCountBuffer[STRING_SIZE];
    cc_string blocksCount = String_FromArray(blocksCountBuffer);

    Format_Int32(&blocksCount, amount);

    if (amount == 1) {
        String_Format1(&message, "&b%s &fblock was cut.", &blocksCount);
    } else {
        String_Format1(&message, "&b%s &fblocks were cut.", &blocksCount);
    }

    Chat_Add(&message);
}

static void DoCut(IVec3 mark1, IVec3 mark2) {
    IVec3 min = VectorUtils_IVec3_Min(mark1, mark2);
    IVec3 max = VectorUtils_IVec3_Max(mark1, mark2);

    Draw_Start("Cut");
    for (int x = min.x; x <= max.x; x++) {
        for (int y = min.y; y <= max.y; y++) {
            for (int z = min.z; z <= max.z; z++) {
                Draw_Block(x, y, z, BLOCK_AIR);
            }
        }
    }

    Draw_End();
}

static void CutSelectionHandler(IVec3* marks, int count) {
    int amountCopied = 0;
    BlocksBuffer_Copy(marks[0], marks[1], &amountCopied);

    DoCut(marks[0], marks[1]);
    ShowBlocksCut(amountCopied);
}

static void Cut_Command(const cc_string* args, int argsCount);

struct ChatCommand CutCommand = {
    "Cut",
    Cut_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Cut",
        "Copies and cut the blocks in an area.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Cut_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(CutCommand);
        return;
    }

    MarkSelection_Make(CutSelectionHandler, 2, "Cut", MACRO_MARKSELECTION_DO_NOT_REPEAT);
    Message_Selection("&aPlace or break two blocks to determine the edges.");
}

#include "ClassiCube/src/Constants.h"

#include "DataStructures/BlocksBuffer.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Format.h"
#include "Memory.h"

static void ShowBlocksCopied(int amount) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    char blocksCountBuffer[STRING_SIZE];
    cc_string blocksCount = String_FromArray(blocksCountBuffer);

    Format_Int32(&blocksCount, amount);

    if (amount == 1) {
        String_Format1(&message, "&b%s &fblock was copied.", &blocksCount);
    } else {
        String_Format1(&message, "&b%s &fblocks were copied.", &blocksCount);
    }

    Chat_Add(&message);
}

static void CopySelectionHandler(IVec3* marks, int count) {
    int amountCopied = 0;
    
    BlocksBuffer_Copy(marks[0], marks[1], &amountCopied);
    ShowBlocksCopied(amountCopied);
}

static void Copy_Command(const cc_string* args, int argsCount) {
    MarkSelection_Make(CopySelectionHandler, 2, "Copy", MACRO_MARKSELECTION_DO_NOT_REPEAT);
    Message_Player("Place or break two blocks to determine the edges.");
}

struct ChatCommand CopyCommand = {
    "Copy",
    Copy_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Copy",
        "Copies the blocks in an area.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

#include <stdio.h>

#include "ClassiCube/src/Constants.h"

#include "DataStructures/BlocksBuffer.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Memory.h"

static void ShowBlocksCopied(int amount) {
    char message[STRING_SIZE];

    if (amount == 1) {
        snprintf(message, sizeof(message), "&b%d &fblock were copied.", amount);
    } else {
        snprintf(message, sizeof(message), "&b%d &fblocks were copied.", amount);
    }

    Message_Player(message);
}

static void CopySelectionHandler(IVec3* marks, int count) {
    int amountCopied = 0;
    
    BlocksBuffer_Copy(marks[0], marks[1], &amountCopied);
    ShowBlocksCopied(amountCopied);
}

static void Copy_Command(const cc_string* args, int argsCount) {
    MarkSelection_Make(CopySelectionHandler, 2, "Copy");
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

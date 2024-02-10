#include <stdio.h>

#include "DataStructures/BlocksBuffer.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Memory.h"

static void Copy_Command(const cc_string* args, int argsCount);
static void CopySelectionHandler(IVec3* marks, int count);
static void ShowBlocksCopied(int amount);

struct ChatCommand CopyCommand = {
    "Copy",
    Copy_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Copy &f- Copies the blocks in an area.",
        NULL,
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void ShowBlocksCopied(int amount) {
    char message[128];

    if (amount == 1) {
        snprintf(message, sizeof(message), "&b%d &fblock were copied.", amount);
    } else {
        snprintf(message, sizeof(message), "&b%d &fblocks were copied.", amount);
    }

    Message_Player(message);
}

static void CopySelectionHandler(IVec3* marks, int count) {
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
    
    ShowBlocksCopied(amountCopied);
}

static void Copy_Command(const cc_string* args, int argsCount) {
    MarkSelection_Make(CopySelectionHandler, 2, "Copy");
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

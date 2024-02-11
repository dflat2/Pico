#include "ClassiCube/src/Constants.h"

#include "Message.h"
#include "UndoTree.h"

static void UndoList_Command(const cc_string* args, int argsCount);

struct ChatCommand UndoListCommand = {
    "UndoList",
    UndoList_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/UndoList",
        "Lists the five most recent terminal operations in the undo tree.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void UndoList_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(UndoListCommand);
        return;
    }

    char buffers[5][STRING_SIZE];
    cc_string descriptions[STRING_SIZE];

    for (int i = 0; i < 5; i++) {
        descriptions[i].buffer = buffers[i];
        descriptions[i].capacity = STRING_SIZE;
        descriptions[i].length = 0;
    }

    int descriptionsCount = 0;
    UndoTree_UndoList(descriptions, &descriptionsCount);

    Message_Player("Undo list:");

    for (int i = 0; i < descriptionsCount; i++) {
        Chat_Add(&descriptions[i]);
    }
}

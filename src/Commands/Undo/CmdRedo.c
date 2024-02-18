#include "Message.h"
#include "UndoTree.h"

static void Redo_Command(const cc_string* args, int argsCount);

struct ChatCommand RedoCommand = {
    "Redo",
    Redo_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Redo",
        "Reverts last &b/Undo&f, &b/Earlier&f, &b/Later&f or &b/Checkout&f.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Redo_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(RedoCommand);
        return;
    }

    UndoTree_Redo();
}

#include "Message.h"
#include "UndoTree.h"
#include "Parse.h"

static void Later_Command(const cc_string* args, int argsCount);

struct ChatCommand LaterCommand = {
    "Later",
    Later_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Later <duration>",
        "Navigates the undo tree in the future.",
        "\x07 &bduration&f: duration string (for example &b1h30m10s&f).",
        NULL,
        NULL
    },
    NULL
};

static void Later_Command(const cc_string* args, int argsCount) {
    if (argsCount != 1) {
        Message_CommandUsage(LaterCommand);
        return;
    }

    int duration_Second;

    if (!Parse_TryParseDeltaTime_Second(&(args[0]), &duration_Second)) {
        Parse_ShowExamplesDeltaTime();
        return;
    }

    UndoTree_Later(duration_Second);
}

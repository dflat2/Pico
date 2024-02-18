#include "Message.h"
#include "UndoTree.h"
#include "Parse.h"
#include "Memory.h"

static void Earlier_Command(const cc_string* args, int argsCount);

struct ChatCommand EarlierCommand = {
    "Earlier",
    Earlier_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Earlier <duration>",
        "Navigates the undo tree in the past.",
        "\x07 &bduration&f: duration string (for example &b1h30m10s&f).",
        NULL,
        NULL
    },
    NULL
};

static void Earlier_Command(const cc_string* args, int argsCount) {
    if (argsCount != 1) {
        Message_CommandUsage(EarlierCommand);
        return;
    }

    int duration_Second;

    if (!Parse_TryParseDeltaTime_Second(&(args[0]), &duration_Second)) {
        Parse_ShowExamplesDeltaTime();
        return;
    }

    UndoTree_Earlier(duration_Second);
}

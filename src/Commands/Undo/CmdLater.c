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
        "Navigates the &bUndoTree &fin the future.",
        NULL,
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

    int commit;
    if (!UndoTree_Later_MALLOC(duration_Second, &commit)) {
        Message_Player("No operation to checkout");
        return;
    } 

    char messageBuffer[STRING_SIZE];
    cc_string message = { messageBuffer, .length = 0, .capacity = STRING_SIZE };
    UndoTree_FormatCurrentNode(&message);
    Chat_AddOf(&message, MSG_TYPE_SMALLANNOUNCEMENT);
}

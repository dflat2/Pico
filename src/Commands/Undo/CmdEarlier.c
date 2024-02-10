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
        "Navigates the &bUndoTree &fin the past.",
        NULL,
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

    int commit;
    bool success = UndoTree_Earlier_MALLOC(duration_Second, &commit);

    if (Memory_AllocationError()) {
        Memory_HandleError();
        Message_MemoryError("running &b/Earlier&f.");
        return;
    }

    if (!success) {
        Message_Player("Already at the earliest moment.");
        return;
    } 

    char messageBuffer[STRING_SIZE];
    cc_string message = { messageBuffer, .length = 0, .capacity = STRING_SIZE };
    UndoTree_FormatCurrentNode(&message);
    Chat_AddOf(&message, MSG_TYPE_SMALLANNOUNCEMENT);
}

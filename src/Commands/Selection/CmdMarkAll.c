#include "ClassiCube/src/World.h"

#include "Message.h"
#include "MarkSelection.h"

static void MarkAll_Command(const cc_string* args, int argsCount);

struct ChatCommand MarkAllCommand = {
    "MarkAll",
    MarkAll_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/MarkAll",
        "Places marks in min and max corners of the map.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void MarkAll_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(MarkAllCommand);
        return;
    }

    if (MarkSelection_RemainingMarks() == 0) {
        Message_Player("Cannot mark, no selection in progress.");
        return;
    }
    else if (MarkSelection_RemainingMarks() == 1) {
        Message_Player("Cannot &b/MarkAll &funless there are at least two remaining marks.");
        return;
    }

    IVec3 low = { .x = 0, .y = 0, .z = 0 };
    IVec3 high = { .x = World.Width - 1, .y = World.Height - 1, .z = World.Length - 1 };

    MarkSelection_DoMark(low);
    MarkSelection_DoMark(high);
}

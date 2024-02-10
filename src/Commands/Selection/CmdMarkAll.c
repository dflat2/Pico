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
        "&fPlaces markers in min and max corners of the map.",
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
        Message_Player("&fCannot mark, no selection in progress.");
        return;
    }
    else if (MarkSelection_RemainingMarks() == 1) {
        Message_Player("&fCannot &b/MarkAll &funless there are at least two remaining marks.");
        return;
    }

    IVec3 low = { .X = 0, .Y = 0, .Z = 0 };
    IVec3 high = { .X = World.Width - 1, .Y = World.Height - 1, .Z = World.Length - 1 };

    MarkSelection_DoMark(low);
    MarkSelection_DoMark(high);
}

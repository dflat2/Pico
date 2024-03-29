#include "DataStructures/BlocksBuffer.h"
#include "Parse.h"
#include "Message.h"
#include "Memory.h"

static void Flip_Command(const cc_string* args, int argsCount);

struct ChatCommand FlipCommand = {
    "Flip",
    Flip_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Flip <axis>",
        "Flips the copied cuboid around the given axis.",
        "\x07 &baxis&f: &bX&f, &bY&f or &bZ&f.",
        NULL,
        NULL
    },
    NULL
};

static void Flip_Command(const cc_string* args, int argsCount) {
    if (BlocksBuffer_IsEmpty()) {
        Message_Player("There is nothing to flip. Do &b/Copy &ffirst.");
        return;
    }

    if (argsCount == 0) {
        Message_CommandUsage(FlipCommand);
        return;
    }
    
    Axis axis;

    if (!Parse_TryParseAxis(&args[0], &axis)) {
        return;
    }

    BlocksBuffer_Flip(axis);

    switch (axis) {
        case AXIS_X:
            Message_Player("Switched around the &bX&f axis.");
            break;
        case AXIS_Y:
            Message_Player("Switched around the &bY&f axis.");
            break;
        default:
            Message_Player("Switched around the &bZ&f axis.");
            break;
    }
}

#include "ClassiCube/src/Constants.h"

#include "DataStructures/BlocksBuffer.h"
#include "Parse.h"
#include "Message.h"
#include "Memory.h"

static void Rotate_Command(const cc_string* args, int argsCount);

struct ChatCommand RotateCommand = {
    "Rotate",
    Rotate_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Rotate [axis] [degrees]",
        "Rotates the copied cuboid.",
        "\x07 &baxis&f: &bX&f, &bY&f (default) or &bZ&f.",
        "\x07 &bdegrees&f: multiple of &b90 &f(default &b90&f).",
        NULL
    },
    NULL
};

static void ShowRotated(Axis axis, int degrees) {
    char charAxis = 'X';

    if (axis == AXIS_Y) {
        charAxis = 'Y';
    } else if (axis == AXIS_Z) {
        charAxis = 'Z';
    }

    char buffer[STRING_SIZE];
    cc_string message = String_FromArray(buffer);
    String_Format2(&message, "Rotated around the %r axis by %i degrees.", &charAxis, &degrees);
    Chat_Add(&message);
}

static void Rotate_Command(const cc_string* args, int argsCount) {
    if (BlocksBuffer_IsEmpty()) {
        Message_Player("There is nothing to rotate. Do &b/Copy &ffirst.");
        return;
    }

    if (argsCount >= 3) {
        Message_CommandUsage(RotateCommand);
        return;
    }

    bool hasAxis = argsCount >= 1;
    bool hasDegrees = argsCount == 2;
    
    Axis axis;
    int degrees;

    if (!hasAxis) {
        axis = AXIS_Y;
    } else if (!Parse_TryParseAxis(&args[0], &axis)) {
        return;
    }

    if (!hasDegrees) {
        degrees = 90;
    } else if (!Parse_TryParseDegrees(&args[1], &degrees)) {
        return;
    }

    BlocksBuffer_Rotate(axis, degrees / 90);
    ShowRotated(axis, degrees);
}

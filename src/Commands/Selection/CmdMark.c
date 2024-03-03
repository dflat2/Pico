#include "ClassiCube/src/World.h"

#include "MarkSelection.h"
#include "Parse.h"
#include "Message.h"
#include "Player.h"

static void Mark_Command(const cc_string* args, int argsCount);

struct ChatCommand MarkCommand = {
    "Mark",
    Mark_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Mark [coordinates]",
        "Places a mark. Parenthesis indicate a relative position.",
        "For example, &b/Mark (0) (2) (0) &fwill mark above you.",
        "\x07 &bcoordinates&f: three space-separated integers.",
        NULL
    },
    NULL
};

static void Mark_Command(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        MarkSelection_DoMark(SnapToWorldBoundaries(Player_GetPosition()));
        return;
    }

    if (argsCount != 3) {
        Message_CommandUsage(MarkCommand);
        return;
    }

    IVec3 target;
    
    if (!Parse_TryParseCoordinates(args, &target)) {
        return;
    }
    
    MarkSelection_DoMark(SnapToWorldBoundaries(target));
}

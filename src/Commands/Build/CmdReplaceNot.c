#include "ClassiCube/src/World.h"

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"

static void ReplaceNot_Command(const cc_string* args, int argsCount);

static bool s_Repeat = false;
static BlockID s_ReplacedNotBlock;

struct ChatCommand ReplaceNotCommand = {
    "ReplaceNot",
    ReplaceNot_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/ReplaceNot <block> @ +",
        "Replaces all blocks but &bblock&f.",
        "\x07 &bblock&f: block name or identifier.",
        NULL,
        NULL
    },
    NULL
};

static void ReplaceSelectionHandler(IVec3* marks, int count);

static void DoReplaceNot(IVec3 min, IVec3 max) {
    Draw_Start("ReplaceNot");

    for (int x = min.X; x <= max.X; x++) {
        for (int y = min.Y; y <= max.Y; y++) {
            for (int z = min.Z; z <= max.Z; z++) {
                if (World_GetBlock(x, y, z) != s_ReplacedNotBlock) {
                    Draw_Brush(x, y, z);
                }
            }
        }
    }

    int blocksAffected = Draw_End();

    if (s_Repeat) {
        MarkSelection_Make(ReplaceSelectionHandler, 2, "ReplaceNot");
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void ReplaceSelectionHandler(IVec3* marks, int count) {
    DoReplaceNot(VectorUtils_IVec3_Min(marks[0], marks[1]), VectorUtils_IVec3_Max(marks[0], marks[1]));
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        Message_CommandUsage(ReplaceNotCommand);
        return false;
    }

    if (!Parse_TryParseBlock(&args[0], &s_ReplacedNotBlock)) {
        return false;
    }

    bool hasBlockOrBrush = (argsCount >= 2);

    if (hasBlockOrBrush) {
        if (!Parse_TryParseBlockOrBrush(&args[1], argsCount - 1)) {
            return false;
        }

        return true;
    } 

    Brush_LoadInventory();
    return true;
}

static void ReplaceNot_Command(const cc_string* args, int argsCount) {
    s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    if (s_Repeat) {
        Message_Player("Now repeating &bReplaceNot&f.");
    }

    MarkSelection_Make(ReplaceSelectionHandler, 2, "ReplaceNot");
    Message_Player("Place or break two blocks to determine the edges.");
}

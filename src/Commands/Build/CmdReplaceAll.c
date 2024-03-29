#include "ClassiCube/src/World.h"

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Parse.h"

static void ReplaceAll_Command(const cc_string* args, int argsCount);

static BlockID s_ReplacedBlock;

struct ChatCommand ReplaceAllCommand = {
    "ReplaceAll",
    ReplaceAll_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/ReplaceAll <block> @",
        "Replaces &bblock &fwith the block you're holding across all map.",
        "\x07 &bblock&f: block name or identifier.",
        NULL,
        NULL,
    },
    NULL
};

static void DoReplaceAll(void) {
    Draw_Start("ReplaceAll");

    for (int x = 0; x <= World.Width - 1; x++) {
        for (int y = 0; y <= World.Height - 1; y++) {
            for (int z = 0; z <= World.Length - 1; z++) {
                if (World_GetBlock(x, y, z) == s_ReplacedBlock) {
                    Draw_Brush(x, y, z);
                }
            }
        }
    }

    int blocksAffected = Draw_End();
    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        Message_CommandUsage(ReplaceAllCommand);
        return false;
    }

    if (!Parse_TryParseBlock(&args[0], &s_ReplacedBlock)) {
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

static void ReplaceAll_Command(const cc_string* args, int argsCount) {
    MarkSelection_Abort();

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    DoReplaceAll();
}

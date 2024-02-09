#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Inventory.h"
#include "ClassiCube/src/World.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Parse.h"

static void ReplaceAll_Command(const cc_string* args, int argsCount);
static bool TryParseArguments(const cc_string* args, int argsCount);
static void DoReplaceAll(void);

static BlockID s_ReplacedBlock;

struct ChatCommand ReplaceAllCommand = {
    "ReplaceAll",
    ReplaceAll_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/ReplaceAll <oldblock> [block/brush]",
        "Replaces &b<oldblock> &fwith &b[block] &facross the whole map.",
        NULL,
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

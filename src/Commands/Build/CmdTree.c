#include "MarkSelection.h"
#include "Message.h"
#include "Parse.h"
#include "Draw.h"

static void Tree_Command(const cc_string* args, int argsCount);

struct ChatCommand TreeCommand = {
    "Tree",
    Tree_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Tree +",
        "Grows a tree.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void DrawTrunk(int x, int y, int z) {
    for (int i = 0; i < 6; i++) {
        Draw_Block(x, y + i, z, BLOCK_LOG);
    }
}

static void DrawLeavesLayer1(int x, int y, int z) {
    int height = y + 3;

    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            if (i == 0 && j == 0) continue;
            if (i == -2 && j == -2) continue;
            if (i == -2 && j == 2) continue;
            if (i == 2 && j == 2) continue;
            if (i == 2 && j == -2) continue;
            Draw_Block(x + i, height, z + j, BLOCK_LEAVES);
        }
    }
}

static void DrawLeavesLayer2(int x, int y, int z) {
    DrawLeavesLayer1(x, y + 1, z);
}

static void DrawLeavesLayer3(int x, int y, int z) {
    int height = y + 5;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            Draw_Block(x + i, height, z + j, BLOCK_LEAVES);
        }
    }
}

static void DrawLeavesLayer4(int x, int y, int z) {
    int height = y + 6;

    Draw_Block(x, height, z, BLOCK_LEAVES);
    Draw_Block(x + 1, height, z, BLOCK_LEAVES);
    Draw_Block(x - 1, height, z, BLOCK_LEAVES);
    Draw_Block(x, height, z + 1, BLOCK_LEAVES);
    Draw_Block(x, height, z - 1, BLOCK_LEAVES);
}

static void TreeSelectionHandler(IVec3* marks, int count) {
    int x = marks[0].x;
    int y = marks[0].y;
    int z = marks[0].z;

    Draw_Start("Tree");
    DrawTrunk(x, y, z);
    DrawLeavesLayer1(x, y, z);
    DrawLeavesLayer2(x, y, z);
    DrawLeavesLayer3(x, y, z);
    DrawLeavesLayer4(x, y, z);
    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break a block to determine the root.");
        MarkSelection_Make(TreeSelectionHandler, 1, "Tree", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Tree_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount > 0) {
        Message_CommandUsage(TreeCommand);
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bTree&f.");
    }

    MarkSelection_Make(TreeSelectionHandler, 1, "Tree", repeat);
    Message_Selection("&aPlace or break a block to determine the root.");
}

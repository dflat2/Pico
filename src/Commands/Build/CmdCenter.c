#include <stdio.h>

#include "Brushes/Brush.h"
#include "Memory.h"
#include "ClassiCube/src/Constants.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"

static void Center_Command(const cc_string* args, int argsCount);

struct ChatCommand CenterCommand = {
    "Center",
    Center_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Center @ +",
        "Places blocks at the center of your selection.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void CenterSelectionHandler(IVec3* marks, int count) {
    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorUtils_IVec3_Max(marks[0], marks[1]);

    IVec3 delta = {
        .x = max.x - min.x,
        .y = max.y - min.y,
        .z = max.z - min.z
    };

    IVec3 centerCuboidMin = {
        .x = (min.x + max.x) / 2,
        .y = (min.y + max.y) / 2,
        .z = (min.z + max.z) / 2
    };

    IVec3 centerCuboidMax = {
        (delta.x % 2) == 0 ? centerCuboidMin.x : (centerCuboidMin.x + 1),
        (delta.y % 2) == 0 ? centerCuboidMin.y : (centerCuboidMin.y + 1),
        (delta.z % 2) == 0 ? centerCuboidMin.z : (centerCuboidMin.z + 1)
    };

    Draw_Start("Center");

    for (int i = centerCuboidMin.x; i <= centerCuboidMax.x; i++) {
        for (int j = centerCuboidMin.y; j <= centerCuboidMax.y; j++) {
            for (int k = centerCuboidMin.z; k <= centerCuboidMax.z; k++) {
                Draw_Brush(i, j, k);
            }
        }
    }
    int blocksAffected = Draw_End();

    char message[STRING_SIZE];

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break two blocks to determine the edges.");
        MarkSelection_Make(CenterSelectionHandler, 2, "Center", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    snprintf(message, sizeof(message), "Drew cuboid from &b(%d, %d, %d)&f to &b(%d, %d, %d)&f.",
             centerCuboidMin.x, centerCuboidMin.y, centerCuboidMin.z,
             centerCuboidMax.x, centerCuboidMax.y, centerCuboidMax.z);

    Message_Player(message);
    Message_BlocksAffected(blocksAffected);
}

static void Center_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);
    
    if (argsCount != 0) {
        if (!Parse_TryParseBlockOrBrush(&args[0], argsCount)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    if (repeat) {
        Message_Player("Now repeating &bCenter&f.");
    }

    MarkSelection_Make(CenterSelectionHandler, 2, "Center", repeat);
    Message_Selection("&aPlace or break two blocks to determine the edges.");
}

#include <stdio.h>

#include "Brushes/Brush.h"
#include "Memory.h"
#include "ClassiCube/src/Constants.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"

static bool s_Repeat = false;

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
    if (count != 2) {
        return;
    }

    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorsUtils_IVec3_Max(marks[0], marks[1]);

    IVec3 delta = {
        .X = max.X - min.X,
        .Y = max.Y - min.Y,
        .Z = max.Z - min.Z
    };

    IVec3 centerCuboidMin = {
        .X = (min.X + max.X) / 2,
        .Y = (min.Y + max.Y) / 2,
        .Z = (min.Z + max.Z) / 2
    };

    IVec3 centerCuboidMax = {
        (delta.X % 2) == 0 ? centerCuboidMin.X : (centerCuboidMin.X + 1),
        (delta.Y % 2) == 0 ? centerCuboidMin.Y : (centerCuboidMin.Y + 1),
        (delta.Z % 2) == 0 ? centerCuboidMin.Z : (centerCuboidMin.Z + 1)
    };

    Draw_Start("Center");

    for (int i = centerCuboidMin.X; i <= centerCuboidMax.X; i++) {
        for (int j = centerCuboidMin.Y; j <= centerCuboidMax.Y; j++) {
            for (int k = centerCuboidMin.Z; k <= centerCuboidMax.Z; k++) {
                Draw_Brush(i, j, k);
            }
        }
    }
    int blocksAffected = Draw_End();

    char message[STRING_SIZE];

    if (s_Repeat) {
        MarkSelection_Make(CenterSelectionHandler, 2, "Center");
        return;
    }

    snprintf(message, sizeof(message), "Drew cuboid from &b(%d, %d, %d)&f to &b(%d, %d, %d)&f.",
             centerCuboidMin.X, centerCuboidMin.Y, centerCuboidMin.Z,
             centerCuboidMax.X, centerCuboidMax.Y, centerCuboidMax.Z);

    Message_Player(message);
    Message_BlocksAffected(blocksAffected);
}

static void Center_Command(const cc_string* args, int argsCount) {
    s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);
    
    if (argsCount != 0) {
        if (!Parse_TryParseBlockOrBrush(&args[0], argsCount)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    if (s_Repeat) {
        Message_Player("Now repeating &bCenter&f.");
    }

    MarkSelection_Make(CenterSelectionHandler, 2, "Center");
    Message_Player("Place or break two blocks to determine the edges.");
}

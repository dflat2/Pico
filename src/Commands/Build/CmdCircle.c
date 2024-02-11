#include <math.h>
#include <stdio.h>

#include "Brushes/Brush.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"
#include "DataStructures/Array.h"
#include "Draw.h"

typedef enum CircleMode_ {
    MODE_SOLID = 0,
    MODE_HOLLOW = 1,
} CircleMode;

static bool s_Repeat = false;
static CircleMode s_Mode;
static Axis s_Axis;
static int s_Radius;

static void Circle_Command(const cc_string* args, int argsCount);

struct ChatCommand CircleCommand = {
    "Circle",
    Circle_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Circle <radius> <axis> [mode] [brush/block] +",
        "Draws a circle of radius &b<radius>&f.",
        "&b<axis> &fmust be &bX&f, &bY&f or &bZ&f.",
        "List of modes: &bsolid&f (default) or &bhollow&f.",
        NULL,
    },
    NULL
};

static void CircleSelectionHandler(IVec3* marks, int count) {
    if (count != 1) {
        return;
    }

    Draw_Start("Circle");

    IVec3 current;
    IVec2 current2D;
    double distance;

    for (int i = -s_Radius; i <= s_Radius; i++) {
        for (int j = -s_Radius; j <= s_Radius; j++) {
            current2D.X = i;
            current2D.Y = j;

            distance = sqrt(VectorUtils_IVec2_DotProduct(current2D, current2D));

            if ((s_Mode == MODE_SOLID && distance <= s_Radius) || (s_Radius - 1 <= distance && distance <= s_Radius)) {
                current = VectorsUtils_IVec3_Add(marks[0], VectorUtils_2DTo3D(current2D, s_Axis));
                Draw_Brush(current.X, current.Y, current.Z);
            }
        }
    }

    int blocksAffected = Draw_End();

    if (s_Repeat) {
        MarkSelection_Make(CircleSelectionHandler, 1, "Circle");
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Circle_Command(const cc_string* args, int argsCount) {
    s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount < 2) {
        Message_CommandUsage(CircleCommand);
        return;
    }

    if (!Parse_TryParseNumber(&args[0], &s_Radius)) {
        return;
    }

    if (s_Radius <= 0) {
        Message_Player("The radius must be positive.");
        return;
    }

    if (!Parse_TryParseAxis(&args[1], &s_Axis)) {
        return;
    }

    cc_string modesString[] = {
        String_FromConst("solid"),
        String_FromConst("hollow"),
    };

    size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

    bool hasMode = (argsCount >= 3) && Array_ContainsString(&args[2], modesString, modesCount);
    bool hasBlockOrBrush = (argsCount >= 4) || ((argsCount == 3) && !hasMode);

    if (hasMode) {
        s_Mode = Array_IndexOfStringCaseless(&args[2], modesString, modesCount);
    } else {
        s_Mode = MODE_SOLID;
    }

    int blockOrBrushIndex = hasMode ? 3 : 2;

    if (hasBlockOrBrush) {
        if (!Parse_TryParseBlockOrBrush(&args[blockOrBrushIndex], argsCount - blockOrBrushIndex)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    if (s_Repeat) {
        Message_Player("Now repeating &bCircle&f.");
    }

    MarkSelection_Make(CircleSelectionHandler, 1, "Circle");
    Message_Player("Place or break a block to determine the center.");
}

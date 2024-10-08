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

static CircleMode s_Mode;
static Axis s_Axis;
static int s_Radius;

static void Circle_Command(const cc_string* args, int argsCount);

struct ChatCommand CircleCommand = {
    "Circle",
    Circle_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Circle <radius> <axis> [mode] @ +",
        "Draws a circle.",
        "\x07 &bradius&f: positive non-zero integer.",
        "\x07 &baxis&f: &bX&f, &bY&f or &bZ&f.",
        "\x07 &bmode&f: &bsolid&f (default) or &bhollow&f.",
    },
    NULL
};

static void CircleSelectionHandler(IVec3* marks, int count) {
    Draw_Start("Circle");

    IVec3 current;
    IVec2 current2D;
    double distance;

    for (int i = -s_Radius; i <= s_Radius; i++) {
        for (int j = -s_Radius; j <= s_Radius; j++) {
            current2D.x = i;
            current2D.y = j;

            distance = sqrt(VectorUtils_IVec2_DotProduct(current2D, current2D));

            if ((s_Mode == MODE_SOLID && distance <= s_Radius) || (s_Radius - 1 <= distance && distance <= s_Radius)) {
                current = VectorsUtils_IVec3_Add(marks[0], VectorUtils_2DTo3D(current2D, s_Axis));
                Draw_Brush(current.x, current.y, current.z);
            }
        }
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break a block to determine the center.");
        MarkSelection_Make(CircleSelectionHandler, 1, "Circle", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Circle_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

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

    if (repeat) {
        Message_Player("Now repeating &bCircle&f.");
    }

    MarkSelection_Make(CircleSelectionHandler, 1, "Circle", repeat);
    Message_Selection("&aPlace or break a block to determine the center.");
}

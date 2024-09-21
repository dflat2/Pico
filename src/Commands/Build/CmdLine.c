#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"
#include "DataStructures/Array.h"

typedef enum LineMode_ {
    MODE_NORMAL,
    MODE_WALL,
    MODE_BEZIER
} LineMode;

static LineMode s_Mode;

static int GreatestInteger2(int a, int b) {
    if (a > b) {
        return a;
    }

    return b;
}

static int GreatestInteger3(int a, int b, int c) {
    return GreatestInteger2(a, GreatestInteger2(b, c));
}

static void Line(IVec3 from, IVec3 to) {
    int deltaX = to.x - from.x;
    int deltaY = to.y - from.y;
    int deltaZ = to.z - from.z;

    int steps = GreatestInteger3(abs(deltaX), abs(deltaY), abs(deltaZ));

    if (steps == 0) {
        return;
    }

    float incrementX = (float)deltaX / (float)steps;
    float incrementY = (float)deltaY / (float)steps;
    float incrementZ = (float)deltaZ / (float)steps;

    float x = from.x;
    float y = from.y;
    float z = from.z;

    for (int _ = 0; _ < steps; _++) {
        Draw_Brush(round(x), round(y), round(z));
        x += incrementX;
        y += incrementY;
        z += incrementZ;
    }

    Draw_Brush(to.x, to.y, to.z);
}

static void DoLine(IVec3 from, IVec3 to) {
    Draw_Start("Line");
    Line(from, to);
}

static void DoWall(IVec3 from, IVec3 to) {
    Draw_Start("Line");

    int deltaX = to.x - from.x;
    int deltaZ = to.z - from.z;

    int steps = GreatestInteger2(abs(deltaX), abs(deltaZ));

    if (steps == 0) {
        printf("deltaX = %d, deltaZ = %d\n", deltaX, deltaZ);
        Draw_End();
        return;
    }

    float incrementX = (float)deltaX / (float)steps;
    float incrementZ = (float)deltaZ / (float)steps;

    float x = from.x;
    float z = from.z;

    int yMin = from.y;
    int yMax = to.y;

    if (from.y > to.y) {
        yMin = to.y;
        yMax = from.y;
    }

    for (int _ = 0; _ < steps; _++) {
        for (int y = yMin; y <= yMax; y++) {
            Draw_Brush(round(x), round(y), round(z));
        }

        x += incrementX;
        z += incrementZ;
    }

    for (int y = yMin; y <= yMax; y++) {
        Draw_Brush(to.x, round(y), to.z);
    }
}

static FVec3 Bezier(FVec3 from, FVec3 controlPoint, FVec3 to, float t) {
    // linear1 = (1 - t) * from + (t) * controlPoint
    FVec3 linear1 = VectorUtils_FVec3_Add(VectorUtils_FVec3_ScalarMultiply(from, 1 - t), VectorUtils_FVec3_ScalarMultiply(controlPoint, t));

    // linear2 = (1 - t) * controlPoint + (t) * to
    FVec3 linear2 = VectorUtils_FVec3_Add(VectorUtils_FVec3_ScalarMultiply(controlPoint, 1 - t), VectorUtils_FVec3_ScalarMultiply(to, t));

    // result = (1 - t) * linear1 + (t) * linear2
    FVec3 result = VectorUtils_FVec3_Add(VectorUtils_FVec3_ScalarMultiply(linear1, 1 - t), VectorUtils_FVec3_ScalarMultiply(linear2, t));

    return result;
}

static void DoBezier(IVec3 from, IVec3 controlPoint, IVec3 to) {
    Draw_Start("Line");
    const int subDivisions = 64;

    FVec3 floatFrom = VectorUtils_IVec3_ConvertFVec3(from);
    FVec3 floatControlPoint = VectorUtils_IVec3_ConvertFVec3(controlPoint);
    FVec3 floatTo = VectorUtils_IVec3_ConvertFVec3(to);

    IVec3 lineStart = from;
    IVec3 lineEnd;

    for (int i = 1; i <= subDivisions; i++) {
        lineEnd = VectorUtils_FVec3_ConvertIVec3(
            Bezier(floatFrom, floatControlPoint, floatTo, i / (float)subDivisions)
        );

        Line(lineStart, lineEnd);
        lineStart = lineEnd;
    }
}

static void LineSelectionHandler(IVec3* marks, int count) {
    if (s_Mode == MODE_NORMAL) {
        DoLine(marks[0], marks[1]);
    } else if (s_Mode == MODE_WALL) {
        DoWall(marks[0], marks[1]);
    } else if (s_Mode == MODE_BEZIER) {
        DoBezier(marks[0], marks[1], marks[2]);
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        if (s_Mode != MODE_BEZIER) {
            Message_Selection("&aPlace or break two blocks to determine the endpoints.");
            MarkSelection_Make(LineSelectionHandler, 2, "Line", MACRO_MARKSELECTION_DO_REPEAT);
        } else {
            Message_Selection("&aPlace or break three blocks.");
            MarkSelection_Make(LineSelectionHandler, 3, "Line (bezier)", MACRO_MARKSELECTION_DO_REPEAT);
        }
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("normal"),
        String_FromConst("wall"),
        String_FromConst("bezier"),
    };

    size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

    bool hasBlockOrBrush = (argsCount >= 2) ||
        ((argsCount == 1) && (!Array_ContainsString(&args[0], modesString, modesCount)));
    bool hasMode = (argsCount >= 1) && Array_ContainsString(&args[0], modesString, modesCount);

    if (hasMode) {
        s_Mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
    } else {
        s_Mode = MODE_NORMAL;
    }

    if (hasMode && (int)s_Mode == -1) {
        Message_ShowUnknownMode(&args[0]);
        Message_ShowAvailableModes(modesString, modesCount);
        return false;
    }

    if (hasBlockOrBrush) {
        int brushIndex;

        if (hasMode) {
            brushIndex = 1;
        } else {
            brushIndex = 0;
        }

        if (!Parse_TryParseBlockOrBrush(&args[brushIndex], argsCount - brushIndex)) {
            return false;
        }

        return true;
    } else {
        Brush_LoadInventory();
        return true;
    }
}

static void Line_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bLine&f.");
    }

    if (s_Mode != MODE_BEZIER) {
        Message_Selection("&aPlace or break two blocks to determine the endpoints.");
    } else {
        Message_Selection("&aPlace or break three blocks.");
    }

    if (s_Mode != MODE_BEZIER) {
            MarkSelection_Make(LineSelectionHandler, 2, "Line", repeat);
        } else {
            MarkSelection_Make(LineSelectionHandler, 3, "Line (bezier)", repeat);
        }
    return;
}

struct ChatCommand LineCommand = {
    "Line",
    Line_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Line [mode] @ +",
        "Draws a line between two points.",
        "\x07 &bmode&f: &bnormal&f (default), &bwall&f, &bbezier&f.",
        NULL,
        NULL
    },
    NULL
};

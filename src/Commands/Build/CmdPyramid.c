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

typedef enum PyramidMode_ {
    MODE_NORMAL,
    MODE_HOLLOW,
} PyramidMode;

static PyramidMode s_Mode;

static void DrawCuboid(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax) {
    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            for (int k = zmin; k <= zmax; k++) {
                Draw_Brush(i, j, k);
            }
        }
    }
}

static void PyramidSelectionHandler(IVec3* marks, int count) {
    int baseY = marks[0].Y;
    
    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorUtils_IVec3_Max(marks[0], marks[1]);

    int sideX = max.X - min.X + 1;
    int sideZ = max.Z - min.Z + 1;

    int height;

    if (sideX > sideZ) {
        height = (sideZ + 1) / 2;
    } else {
        height = (sideX + 1) / 2;
    }

    Draw_Start("Pyramid");

    // Base
    for (int x = min.X; x <= max.X; x++) {
        for (int z = min.Z; z <= max.Z; z++) {
            Draw_Brush(x, baseY, z);
        }
    }

    int xMin = min.X + 1;
    int zMin = min.Z + 1;
    int xMax = max.X - 1;
    int zMax = max.Z - 1;

    for (int i = 1; i < height; i++) {
        if (s_Mode == MODE_NORMAL) {
            DrawCuboid(xMin, baseY + i, zMin, xMax, baseY + i, zMax);
        } else {
            DrawCuboid(xMin, baseY + i, zMin, xMin, baseY + i, zMax);
            DrawCuboid(xMin, baseY + i, zMin, xMax, baseY + i, zMin);
            DrawCuboid(xMax, baseY + i, zMin, xMax, baseY + i, zMax);
            DrawCuboid(xMin, baseY + i, zMax, xMax, baseY + i, zMax);
        }
        
        xMin++;
        zMin++;
        xMax--;
        zMax--;
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        MarkSelection_Make(PyramidSelectionHandler, 2, "Pyramid", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("normal"),
        String_FromConst("hollow"),
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

static void Pyramid_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bPyramid&f.");
    }

    Message_Player("Place or break two blocks to indicate the base.");
    MarkSelection_Make(PyramidSelectionHandler, 2, "Pyramid", repeat);
}

struct ChatCommand PyramidCommand = {
    "Pyramid",
    Pyramid_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Pyramid [mode] @ +",
        "Draws a pyramid from a rectangular base.",
        "\x07 &bmode&f: &bnormal&f (default) or &bhollow&f.",
        NULL,
        NULL
    },
    NULL
};

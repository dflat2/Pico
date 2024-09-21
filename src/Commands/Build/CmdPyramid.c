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
    MODE_ROOF,
    MODE_WIRE,
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
    int baseY = marks[0].y;
    
    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorUtils_IVec3_Max(marks[0], marks[1]);

    int sideX = max.x - min.x + 1;
    int sideZ = max.z - min.z + 1;

    int height;

    if (sideX > sideZ) {
        height = (sideZ + 1) / 2;
    } else {
        height = (sideX + 1) / 2;
    }

    Draw_Start("Pyramid");

    if (s_Mode == MODE_NORMAL || s_Mode == MODE_HOLLOW) {
        DrawCuboid(min.x, baseY, min.z, max.x, baseY, max.z);
    } else if (s_Mode == MODE_ROOF || s_Mode == MODE_WIRE) {
        DrawCuboid(min.x, baseY, min.z, min.x, baseY, max.z);
        DrawCuboid(min.x, baseY, min.z, max.x, baseY, min.z);
        DrawCuboid(max.x, baseY, min.z, max.x, baseY, max.z);
        DrawCuboid(min.x, baseY, max.z, max.x, baseY, max.z);
    }


    int xMin = min.x + 1;
    int zMin = min.z + 1;
    int xMax = max.x - 1;
    int zMax = max.z - 1;

    for (int i = 1; i < height; i++) {
        if (s_Mode == MODE_NORMAL) {
            DrawCuboid(xMin, baseY + i, zMin, xMax, baseY + i, zMax);
        } else if (s_Mode == MODE_HOLLOW || s_Mode == MODE_ROOF) {
            DrawCuboid(xMin, baseY + i, zMin, xMin, baseY + i, zMax);
            DrawCuboid(xMin, baseY + i, zMin, xMax, baseY + i, zMin);
            DrawCuboid(xMax, baseY + i, zMin, xMax, baseY + i, zMax);
            DrawCuboid(xMin, baseY + i, zMax, xMax, baseY + i, zMax);
        } else if (s_Mode == MODE_WIRE) {
            if (i == height - 1) {
                DrawCuboid(xMin, baseY + i, zMin, xMax, baseY + i, zMax);
            } else {
                Draw_Brush(xMin, baseY + i, zMin);
                Draw_Brush(xMin, baseY + i, zMax);
                Draw_Brush(xMax, baseY + i, zMin);
                Draw_Brush(xMax, baseY + i, zMax);
            }
        }
        
        xMin++;
        zMin++;
        xMax--;
        zMax--;
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break two blocks to indicate the base.");
        MarkSelection_Make(PyramidSelectionHandler, 2, "Pyramid", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("normal"),
        String_FromConst("hollow"),
        String_FromConst("roof"),
        String_FromConst("wire"),
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

    Message_Selection("&aPlace or break two blocks to indicate the base.");
    MarkSelection_Make(PyramidSelectionHandler, 2, "Pyramid", repeat);
}

struct ChatCommand PyramidCommand = {
    "Pyramid",
    Pyramid_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Pyramid [mode] @ +",
        "Draws a pyramid from a rectangular base.",
        "\x07 &bmode&f: &bnormal&f (default), &bhollow&f, &broof &for &bwire&f.",
        NULL,
        NULL
    },
    NULL
};

#include <stdlib.h>

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"
#include "DataStructures/Array.h"

typedef enum ZMode_ {
    MODE_SOLID = 0,
    MODE_HOLLOW = 1,
    MODE_WALLS = 2,
    MODE_WIRE = 3,
    MODE_CORNERS = 4
} ZMode;

static ZMode s_Mode;

typedef void (*CuboidOperation)(IVec3 min, IVec3 max);

static void Z_Command(const cc_string* args, int argsCount);

struct ChatCommand ZCommand = {
    "Z",
    Z_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Z [mode] @ +",
        "Draws a cuboid between two points.",
        "\x07 &bmode&f: &bsolid&f (default), &bhollow&f, &bwalls&f, &bwire&f, &bcorners&f.",
        NULL,
        NULL
    },
    NULL
};

static void DrawCuboid(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax) {
    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            for (int k = zmin; k <= zmax; k++) {
                Draw_Brush(i, j, k);
            }
        }
    }
}

static void DoCuboidSolid(IVec3 min, IVec3 max) {
    Draw_Start("Z");
    DrawCuboid(min.x, min.y, min.z, max.x, max.y, max.z);

    int blocksAffected = Draw_End();

    if (!MarkSelection_Repeating()) {
        Message_BlocksAffected(blocksAffected);
    }
}

static void DoCuboidHollow(IVec3 min, IVec3 max) {
    Draw_Start("Z");
    DrawCuboid(min.x, min.y, min.z, min.x, max.y, max.z);
    DrawCuboid(min.x, min.y, min.z, max.x, max.y, min.z);
    DrawCuboid(min.x, min.y, min.z, max.x, min.y, max.z);
    DrawCuboid(max.x, min.y, min.z, max.x, max.y, max.z);
    DrawCuboid(min.x, max.y, min.z, max.x, max.y, max.z);
    DrawCuboid(min.x, min.y, max.z, max.x, max.y, max.z);

    int blocksAffected = Draw_End();

    if (!MarkSelection_Repeating()) {
        Message_BlocksAffected(blocksAffected);
    }
}

static void DoCuboidWalls(IVec3 min, IVec3 max) {
    Draw_Start("Z");
    DrawCuboid(min.x, min.y, min.z, min.x, max.y, max.z);
    DrawCuboid(min.x, min.y, min.z, max.x, max.y, min.z);
    DrawCuboid(max.x, min.y, min.z, max.x, max.y, max.z);
    DrawCuboid(min.x, min.y, max.z, max.x, max.y, max.z);

    int blocksAffected = Draw_End();

    if (!MarkSelection_Repeating()) {
        Message_BlocksAffected(blocksAffected);
    }
}

static void DoCuboidWire(IVec3 min, IVec3 max) {
    Draw_Start("Z");
    DrawCuboid(min.x, min.y, min.z, max.x, min.y, min.z);
    DrawCuboid(min.x, min.y, min.z, min.x, max.y, min.z);
    DrawCuboid(min.x, min.y, min.z, min.x, min.y, max.z);
    DrawCuboid(min.x, min.y, max.z, max.x, min.y, max.z);
    DrawCuboid(min.x, min.y, max.z, min.x, max.y, max.z);
    DrawCuboid(min.x, max.y, min.z, max.x, max.y, min.z);
    DrawCuboid(min.x, max.y, min.z, min.x, max.y, max.z);
    DrawCuboid(min.x, max.y, max.z, max.x, max.y, max.z);
    DrawCuboid(max.x, min.y, min.z, max.x, max.y, min.z);
    DrawCuboid(max.x, min.y, min.z, max.x, min.y, max.z);
    DrawCuboid(max.x, min.y, max.z, max.x, max.y, max.z);
    DrawCuboid(max.x, max.y, min.z, max.x, max.y, max.z);

    Draw_End();
}

static void DoCuboidCorners(IVec3 min, IVec3 max) {
    Draw_Start("Z");
    Draw_Brush(min.x, min.y, min.z);
    Draw_Brush(min.x, min.y, max.z);
    Draw_Brush(min.x, max.y, min.z);
    Draw_Brush(min.x, max.y, max.z);
    Draw_Brush(max.x, min.y, min.z);
    Draw_Brush(max.x, min.y, max.z);
    Draw_Brush(max.x, max.y, min.z);
    Draw_Brush(max.x, max.y, max.z);

    int blocksAffected = Draw_End();

    if (!MarkSelection_Repeating()) {
        Message_BlocksAffected(blocksAffected);
    }
}

static CuboidOperation GetFunction(char mode) {
    switch (mode) {
        case MODE_SOLID:
            return DoCuboidSolid;
        case MODE_HOLLOW:
            return DoCuboidHollow;
        case MODE_WALLS:
            return DoCuboidWalls;
        case MODE_WIRE:
            return DoCuboidWire;
        case MODE_CORNERS:
            return DoCuboidCorners;
        default:
            return DoCuboidSolid;
    };
}

static void ZSelectionHandler(IVec3* marks, int count) {
    CuboidOperation Operation = GetFunction(s_Mode);
    Operation(VectorUtils_IVec3_Min(marks[0], marks[1]), VectorUtils_IVec3_Max(marks[0], marks[1]));

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break two blocks to determine the edges.");
        MarkSelection_Make(ZSelectionHandler, 2, "Z", MACRO_MARKSELECTION_DO_REPEAT);
    }
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("solid"),
        String_FromConst("hollow"),
        String_FromConst("walls"),
        String_FromConst("wire"),
        String_FromConst("corners"),
    };

    size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

    bool hasBlockOrBrush = (argsCount >= 2) ||
        ((argsCount == 1) && (!Array_ContainsString(&args[0], modesString, modesCount)));
    bool hasMode = (argsCount >= 1) && Array_ContainsString(&args[0], modesString, modesCount);

    if (hasMode) {
        s_Mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
    } else {
        s_Mode = MODE_SOLID;
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

static void Z_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        MarkSelection_Abort();
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bZ&f.");
    }

    MarkSelection_Make(ZSelectionHandler, 2, "Z", repeat);
    Message_Selection("&aPlace or break two blocks to determine the edges.");
}

#include <stdio.h>

#include "ClassiCube/src/World.h"

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Memory.h"
#include "Parse.h"
#include "VectorUtils.h"
#include "DataStructures/Array.h"
#include "DataStructures/IVec3FastQueue.h"

typedef enum OutlineMode_ {
    MODE_3D = 0,
    MODE_LAYER = 1,
    MODE_UP = 2,
    MODE_DOWN = 3,
} OutlineMode;

static BlockID s_OutlinedBlock;
static OutlineMode s_Mode;

static void Outline_Command(const cc_string* args, int argsCount);

struct ChatCommand OutlineCommand = {
    "Outline",
    Outline_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Outline <mode> <block> @ +",
        "Outlines &bblock&f. Only air blocks can be affected.",
        "\x07 &bmode&f: &b3d&f, &blayer&f, &bup&f or &bdown&f.",
        "\x07 &bblock&f: block name or identifier.",
        NULL
    },
    NULL
};

static bool TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount <= 1) {
        Message_CommandUsage(OutlineCommand);
        return false;
    }

    cc_string modesString[] = {
        String_FromConst("3d"),
        String_FromConst("layer"),
        String_FromConst("up"),
        String_FromConst("down"),
    };

    size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);
    s_Mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);

    if ((int)s_Mode == -1) {
        Message_ShowUnknownMode(&args[0]);
        Message_ShowAvailableModes(modesString, modesCount);
        return false;
    }

    if (!Parse_TryParseBlock(&args[1], &s_OutlinedBlock)) {
        return false;
    }

    bool hasBlockOrBrush = (argsCount >= 3);

    if (hasBlockOrBrush) {
        if (!Parse_TryParseBlockOrBrush(&args[2], argsCount - 2)) {
            return false;
        }

        return true;
    } else {
        Brush_LoadInventory();
        return true;
    }
}

static bool ShouldOutline(IVec3 coordinates, IVec3 min, IVec3 max) {
    IVec3 neighbors[6];
    int count = 0;
    
    if (s_Mode == MODE_LAYER || s_Mode == MODE_3D) {
        neighbors[count].X = coordinates.X + 1;
        neighbors[count].Y = coordinates.Y;
        neighbors[count].Z = coordinates.Z;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }

        neighbors[count].X = coordinates.X - 1;
        neighbors[count].Y = coordinates.Y;
        neighbors[count].Z = coordinates.Z;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }
        
        neighbors[count].X = coordinates.X;
        neighbors[count].Y = coordinates.Y;
        neighbors[count].Z = coordinates.Z + 1;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }

        neighbors[count].X = coordinates.X;
        neighbors[count].Y = coordinates.Y;
        neighbors[count].Z = coordinates.Z - 1;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }
    }
    
    if (s_Mode == MODE_UP || s_Mode == MODE_3D) {
        neighbors[count].X = coordinates.X;
        neighbors[count].Y = coordinates.Y - 1;
        neighbors[count].Z = coordinates.Z;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }
    }
    
    if (s_Mode == MODE_DOWN || s_Mode == MODE_3D) {
        neighbors[count].X = coordinates.X;
        neighbors[count].Y = coordinates.Y - 1;
        neighbors[count].Z = coordinates.Z;

        if (World_Contains(neighbors[count].X, neighbors[count].Y, neighbors[count].Z)) {
            count++;
        }
    }

    for (int i = 0; i < count; i++) {
        if (World_GetBlock(neighbors[i].X, neighbors[i].Y, neighbors[i].Z) == s_OutlinedBlock) {
            return true;
        }
    }

    return false;
}

static void OutlineSelectionHandler(IVec3* marks, int count) {
    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorUtils_IVec3_Max(marks[0], marks[1]);

    IVec3FastQueue* outline = IVec3FastQueue_CreateEmpty();

    IVec3 here;

    for (int x = min.X; x <= max.X; x++) {
        for (int y = min.Y; y <= max.Y; y++) {
            for (int z = min.Z; z <= max.Z; z++) {
                if (World_GetBlock(x, y, z) == BLOCK_AIR) {
                    here.X = x;
                    here.Y = y;
                    here.Z = z;

                    if (ShouldOutline(here, min, max)) {
                        IVec3FastQueue_Enqueue(outline, here);
                    }
                }
            }
        }
    }

    Draw_Start("Outline");

    while (!IVec3FastQueue_IsEmpty(outline)) {
        here = IVec3FastQueue_Dequeue(outline);
        Draw_Brush(here.X, here.Y, here.Z);
    }

    IVec3FastQueue_Free(outline);

    int blocksAffected = Draw_End();
    Message_BlocksAffected(blocksAffected);

    if (MarkSelection_Repeating()) {
        MarkSelection_Make(OutlineSelectionHandler, 2, "Outline", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Outline_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        MarkSelection_Abort();
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bOutline&f.");
    }

    MarkSelection_Make(OutlineSelectionHandler, 2, "Outline", repeat);
    Message_Player("Place or break two blocks to determine the edges.");
}

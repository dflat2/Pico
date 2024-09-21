#include <stdio.h>

#include "ClassiCube/src/World.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Parse.h"
#include "Memory.h"
#include "Brushes/Brush.h"
#include "DataStructures/Array.h"
#include "DataStructures/BinaryMap.h"
#include "DataStructures/IVec3FastQueue.h"

#define MAX_NEIGHBORS 6

typedef enum FillMode_ {
    MODE_3D = 0,
    MODE_2DX = 1,
    MODE_LAYER = 2,
    MODE_2DZ = 3,
    MODE_DOWN = 4,
    MODE_UP = 5
} FillMode;

static FillMode s_Mode;
static int s_SourceY;

static bool TryParseArguments(const cc_string* args, int argsCount) {
    cc_string modesString[] = {
        String_FromConst("3d"),
        String_FromConst("2d-x"),
        String_FromConst("layer"),
        String_FromConst("2d-z"),
        String_FromConst("down"),
        String_FromConst("up"),
    };

    size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

    bool hasBlockOrBrush = (argsCount >= 2) ||
        ((argsCount == 1) && (!Array_ContainsString(&args[0], modesString, modesCount)));
    bool hasMode = (argsCount >= 1) && Array_ContainsString(&args[0], modesString, modesCount);

    if (hasMode) {
        s_Mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
    } else {
        s_Mode = MODE_3D;
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

static void Expand(IVec3FastQueue* queue, IVec3 target, BlockID filledOverBlock, BinaryMap* binaryMap) {
    IVec3 neighbors[MAX_NEIGHBORS];
    short count = 0;

    if (s_Mode == MODE_3D || s_Mode == MODE_LAYER || s_Mode == MODE_2DZ || s_Mode == MODE_DOWN || s_Mode == MODE_UP) {
        neighbors[count].x = target.x - 1;
        neighbors[count].y = target.y;
        neighbors[count].z = target.z;
        count++;
        neighbors[count].x = target.x + 1;
        neighbors[count].y = target.y;
        neighbors[count].z = target.z;
        count++;
    }
    
    if (s_Mode == MODE_3D || s_Mode == MODE_2DX || s_Mode == MODE_2DZ) {
        neighbors[count].x = target.x;
        neighbors[count].y = target.y - 1;
        neighbors[count].z = target.z;
        count++;
        neighbors[count].x = target.x;
        neighbors[count].y = target.y + 1;
        neighbors[count].z = target.z;
        count++;
    } else if (s_Mode == MODE_DOWN) {
        neighbors[count].x = target.x;
        neighbors[count].y = target.y - 1;
        neighbors[count].z = target.z;
        count++;

        if (target.y < s_SourceY) {
            neighbors[count].x = target.x;
            neighbors[count].y = target.y + 1;
            neighbors[count].z = target.z;
            count++;
        }
    } else if (s_Mode == MODE_UP) {
        neighbors[count].x = target.x;
        neighbors[count].y = target.y + 1;
        neighbors[count].z = target.z;
        count++;

        if (target.y > s_SourceY) {
            neighbors[count].x = target.x;
            neighbors[count].y = target.y - 1;
            neighbors[count].z = target.z;
            count++;
        }
    }
    
    if (s_Mode == MODE_3D || s_Mode == MODE_2DX || s_Mode == MODE_LAYER || s_Mode == MODE_DOWN || s_Mode == MODE_UP) {
        neighbors[count].x = target.x;
        neighbors[count].y = target.y;
        neighbors[count].z = target.z - 1;
        count++;
        neighbors[count].x = target.x;
        neighbors[count].y = target.y;
        neighbors[count].z = target.z + 1;
        count++;
    }

    IVec3 neighbor;

    for (int i = 0; i < count; i++) {
        neighbor = neighbors[i];

        if (!World_Contains(neighbor.x, neighbor.y, neighbor.z) ||
            World_GetBlock(neighbor.x, neighbor.y, neighbor.z) != filledOverBlock ||
            BinaryMap_Get(binaryMap, neighbor.x, neighbor.y, neighbor.z)) {
            continue;
        }

        IVec3FastQueue_Enqueue(queue, neighbor);
        BinaryMap_Set(binaryMap, neighbor.x, neighbor.y, neighbor.z);
    }
}

static void FillSelectionHandler(IVec3* marks, int count) {
    IVec3 fillOrigin = marks[0];
    s_SourceY = fillOrigin.y;

    BlockID filledOverBlock = World_GetBlock(fillOrigin.x, fillOrigin.y, fillOrigin.z);

    BinaryMap* binaryVisitedMap = BinaryMap_CreateEmpty(World.Width, World.Height, World.Length);
    BinaryMap_Set(binaryVisitedMap, fillOrigin.x, fillOrigin.y, fillOrigin.z);

    IVec3FastQueue* queue = IVec3FastQueue_CreateEmpty();
    IVec3FastQueue_Enqueue(queue, fillOrigin);

    Draw_Start("Fill");
    IVec3 current;

    while (!IVec3FastQueue_IsEmpty(queue)) {
        current = IVec3FastQueue_Dequeue(queue);
        Draw_Brush(current.x, current.y, current.z);
        Expand(queue, current, filledOverBlock, binaryVisitedMap);
    }

    BinaryMap_Free(binaryVisitedMap);
    IVec3FastQueue_Free(queue);

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break a block.");
        MarkSelection_Make(FillSelectionHandler, 1, "Fill", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Fill_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        MarkSelection_Abort();
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bFill&f.");
    }

    MarkSelection_Make(FillSelectionHandler, 1, "Fill", repeat);
    Message_Selection("&aPlace or break a block.");
}

struct ChatCommand FillCommand = {
    "Fill",
    Fill_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Fill [mode] @ +",
        "Fills the specified area.",
        "\x07 &bmode&f: &b3d&f (default), &b2d-x&f, &blayer&f, &b2d-z&f, &bdown&f, &bup&f.",
        NULL,
        NULL
    },
    NULL
};

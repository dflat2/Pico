#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Core.h"
#include "CC_API/Game.h"
#include "CC_API/Inventory.h"

#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "ParsingUtils.h"
#include "Axis.h"

typedef enum CircleMode_ {
    SOLID,
    HOLLOW,
} CircleMode;

typedef struct CircleArgs_ {
    CircleMode mode;
    BlockID block;
} CircleArgs;

typedef void (*CircleOperation)(IVec3 center, Axis axis, int radius, BlockID block);

static void MidPointCircleOctant2D(int radius, IVec2* out_octant, int* out_count) {
    *out_count = 0;

    IVec2 here = { .X = radius, .Y = 0 };
    IVec2 northEast;
    IVec2 north;
    float midPointX;
    float midPointY;
    bool isMidPointInCircle;

    while (here.X != here.Y) {
        out_octant[*out_count] = here;
        (*out_count)++;

        northEast.X = here.X - 1;
        northEast.Y = here.Y + 1;
        north.X = here.X;
        north.Y = here.Y + 1;

        midPointX = ((float)northEast.X + (float)north.X) / 2.0;
        midPointY = here.Y + 1;

        isMidPointInCircle = (midPointX * midPointX + midPointY * midPointY) <= (radius * radius);

        if (isMidPointInCircle) {
            here = north;
        }
        else {
            here = northEast;
        }
    }

    out_octant[*out_count] = here;
    (*out_count)++;
}

static void DoCircleHollow(IVec3 center, Axis axis, int radius, BlockID block) {
    // `radius` is an upper bound for the number of blocks in an octant.
    IVec2* octant = allocate(radius, sizeof(IVec2));
    int count = 0;
    MidPointCircleOctant2D(radius, octant, &count);

    IVec3 here;

    for (int i = 0; i < count; i++) {
        here = Add(center, Transform2DTo3D(octant[i], axis));
        Game_UpdateBlock(here.X, here.Y, here.Z, block);
    }
}

static void DoCircleSolid(IVec3 center, Axis axis, int radius, BlockID block) {
    // Not implemented, call DoCircleHollow atm
    DoCircleHollow(center, axis, radius, block);
}

static CircleOperation GetFunction(CircleMode mode) {
    switch (mode) {
        case SOLID:
            return DoCircleSolid;
        case HOLLOW:
            return DoCircleHollow;
        default:
            return DoCircleSolid;
    };
}

static void CircleSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }

    CircleArgs* circleArgs = (CircleArgs*)object;
    CircleOperation Operation = GetFunction(circleArgs->mode);

    int radius = floor(Distance(marks[0], marks[1]));

    //TODO
    //Operation(, circleArgs->block);
}

static void CleanResources(void* args) {
	CircleArgs* circleArgs = (CircleArgs*)args;
	free(circleArgs);
}

static void Circle_Command(const cc_string* args, int argsCount) {
    CircleArgs* circleArgs = (CircleArgs*) allocateZeros(1, sizeof(CircleArgs));

    cc_string modesString[] = {
        String_FromConst(":solid"),
        String_FromConst(":hollow"),
    };

    int modesCount = 2;
    int i_mode;

    if (!TryParseModeBlock((cc_string*)modesString, modesCount, args, argsCount, &i_mode, &circleArgs->block)) {
        return;
    }

    circleArgs->mode = (CircleMode)i_mode;
    MakeSelection(CircleSelectionHandler, 2, circleArgs, CleanResources);
    PlayerMessage("&fPlace or break two blocks to determine the edges.");
}

struct ChatCommand CircleCommand = {
	"Circle",
	Circle_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Circle [mode] [brush | block]",
        "&fDraws a circle.",
		"&fList of modes: &b:solid&f (default), &b:hollow&f.",
		NULL,
		NULL
	},
	NULL
};

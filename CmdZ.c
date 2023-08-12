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

typedef enum ZMode_ {
    SOLID,
    HOLLOW,
    WALLS,
    WIRE,
    CORNERS,
} ZMode;

typedef struct ZArgs_ {
    ZMode mode;
    BlockID block;
} ZArgs;

typedef void (*CuboidOperation)(IVec3 min, IVec3 max, BlockID block);

static void DrawCuboid(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax, BlockID block) {
    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            for (int k = zmin; k <= zmax; k++) {
                Game_UpdateBlock(i, j, k, block);
            }
        }
    }
}

static void DoCuboidSolid(IVec3 min, IVec3 max, BlockID block) {
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, max.Z, block);
}

static void DoCuboidHollow(IVec3 min, IVec3 max, BlockID block) {
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, min.Z, block);
    DrawCuboid(min.X, min.Y, min.Z, max.X, min.Y, max.Z, block);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, max.Z, block);
    DrawCuboid(min.X, max.Y, min.Z, max.X, max.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, max.Z, max.X, max.Y, max.Z, block);
}

static void DoCuboidWalls(IVec3 min, IVec3 max, BlockID block) {
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, min.Z, block);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, max.Z, max.X, max.Y, max.Z, block);
}

static void DoCuboidWire(IVec3 min, IVec3 max, BlockID block) {
    DrawCuboid(min.X, min.Y, min.Z, max.X, min.Y, min.Z, block);
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, min.Z, block);
    DrawCuboid(min.X, min.Y, min.Z, min.X, min.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, max.Z, max.X, min.Y, max.Z, block);
    DrawCuboid(min.X, min.Y, max.Z, min.X, max.Y, max.Z, block);
    DrawCuboid(min.X, max.Y, min.Z, max.X, max.Y, min.Z, block);
    DrawCuboid(min.X, max.Y, min.Z, min.X, max.Y, max.Z, block);
    DrawCuboid(min.X, max.Y, max.Z, max.X, max.Y, max.Z, block);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, min.Z, block);
    DrawCuboid(max.X, min.Y, min.Z, max.X, min.Y, max.Z, block);
    DrawCuboid(max.X, min.Y, max.Z, max.X, max.Y, max.Z, block);
    DrawCuboid(max.X, max.Y, min.Z, max.X, max.Y, max.Z, block);
}

static void DoCuboidCorners(IVec3 min, IVec3 max, BlockID block) {
    Game_UpdateBlock(min.X, min.Y, min.Z, block);
    Game_UpdateBlock(min.X, min.Y, max.Z, block);
    Game_UpdateBlock(min.X, max.Y, min.Z, block);
    Game_UpdateBlock(min.X, max.Y, max.Z, block);
    Game_UpdateBlock(max.X, min.Y, min.Z, block);
    Game_UpdateBlock(max.X, min.Y, max.Z, block);
    Game_UpdateBlock(max.X, max.Y, min.Z, block);
    Game_UpdateBlock(max.X, max.Y, max.Z, block);
}

static CuboidOperation GetFunction(ZMode mode) {
    switch (mode) {
        case SOLID:
            return DoCuboidSolid;
        case HOLLOW:
            return DoCuboidHollow;
        case WALLS:
            return DoCuboidWalls;
        case WIRE:
            return DoCuboidWire;
        case CORNERS:
            return DoCuboidCorners;
        default:
            return DoCuboidSolid;
    };
}

static void ZSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }

    ZArgs* zargs = (ZArgs*)object;
    CuboidOperation Operation = GetFunction(zargs->mode);
    Operation(Min(marks[0], marks[1]), Max(marks[0], marks[1]), zargs->block);
}

static void CleanResources(void* args) {
	ZArgs* zargs = (ZArgs*)args;
	free(zargs);
}

static void Z_Command(const cc_string* args, int argsCount) {
    ZArgs* zargs = (ZArgs*) allocateZeros(1, sizeof(ZArgs));

    cc_string modesString[] = {
        String_FromConst(":solid"),
        String_FromConst(":hollow"),
        String_FromConst(":walls"),
        String_FromConst(":wire"),
        String_FromConst(":corners"),
    };

    int modesCount = 5;
    int i_mode;

    if (!TryParseModeBlock((cc_string*)modesString, modesCount, args, argsCount, &i_mode, &zargs->block)) {
        return;
    }

    zargs->mode = (ZMode)i_mode;
    MakeSelection(ZSelectionHandler, 2, zargs, CleanResources);
    PlayerMessage("&fPlace or break two blocks to determine the edges.");
}

struct ChatCommand ZCommand = {
	"Z",
	Z_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Z [mode] [brush | block]",
        "&fDraws a cuboid between two points.",
		"&fList of modes: &b:solid&f (default), &b:hollow&f, &b:walls&f, &b:wire&f, &b:corners&f.",
		NULL,
		NULL
	},
	NULL
};

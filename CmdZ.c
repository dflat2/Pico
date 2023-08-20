#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "ParsingUtils.h"
#include "Array.h"

typedef char ZMode;

typedef struct ZArguments_ {
	ZMode mode;
	Brush* brush;
} ZArguments;

static const ZMode SOLID_MODE = 0;
static const ZMode HOLLOW_MODE = 1;
static const ZMode WALLS_MODE = 2;
static const ZMode WIRE_MODE = 3;
static const ZMode CORNERS_MODE = 4;

typedef void (*CuboidOperation)(IVec3 min, IVec3 max, Brush* brush);

static void DrawCuboid(int xmin, int ymin, int zmin, int xmax, int ymax, int zmax, Brush* brush) {
    for (int i = xmin; i <= xmax; i++) {
        for (int j = ymin; j <= ymax; j++) {
            for (int k = zmin; k <= zmax; k++) {
                Draw_Brush(i, j, k, brush);
            }
        }
    }
}

static void DoCuboidSolid(IVec3 min, IVec3 max, Brush* brush) {
	Draw_Start("Cuboid solid");
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, max.Z, brush);

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void DoCuboidHollow(IVec3 min, IVec3 max, Brush* brush) {
	Draw_Start("Cuboid hollow");
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, min.Z, brush);
    DrawCuboid(min.X, min.Y, min.Z, max.X, min.Y, max.Z, brush);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, max.Y, min.Z, max.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, max.Z, max.X, max.Y, max.Z, brush);

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void DoCuboidWalls(IVec3 min, IVec3 max, Brush* brush) {
	Draw_Start("Cuboid walls");
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, min.Z, max.X, max.Y, min.Z, brush);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, max.Z, max.X, max.Y, max.Z, brush);

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void DoCuboidWire(IVec3 min, IVec3 max, Brush* brush) {
	Draw_Start("Cuboid wire");
    DrawCuboid(min.X, min.Y, min.Z, max.X, min.Y, min.Z, brush);
    DrawCuboid(min.X, min.Y, min.Z, min.X, max.Y, min.Z, brush);
    DrawCuboid(min.X, min.Y, min.Z, min.X, min.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, max.Z, max.X, min.Y, max.Z, brush);
    DrawCuboid(min.X, min.Y, max.Z, min.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, max.Y, min.Z, max.X, max.Y, min.Z, brush);
    DrawCuboid(min.X, max.Y, min.Z, min.X, max.Y, max.Z, brush);
    DrawCuboid(min.X, max.Y, max.Z, max.X, max.Y, max.Z, brush);
    DrawCuboid(max.X, min.Y, min.Z, max.X, max.Y, min.Z, brush);
    DrawCuboid(max.X, min.Y, min.Z, max.X, min.Y, max.Z, brush);
    DrawCuboid(max.X, min.Y, max.Z, max.X, max.Y, max.Z, brush);
    DrawCuboid(max.X, max.Y, min.Z, max.X, max.Y, max.Z, brush);

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void DoCuboidCorners(IVec3 min, IVec3 max, Brush* brush) {
	Draw_Start("Cuboid corners");
    Draw_Brush(min.X, min.Y, min.Z, brush);
    Draw_Brush(min.X, min.Y, max.Z, brush);
    Draw_Brush(min.X, max.Y, min.Z, brush);
    Draw_Brush(min.X, max.Y, max.Z, brush);
    Draw_Brush(max.X, min.Y, min.Z, brush);
    Draw_Brush(max.X, min.Y, max.Z, brush);
    Draw_Brush(max.X, max.Y, min.Z, brush);
    Draw_Brush(max.X, max.Y, max.Z, brush);

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static CuboidOperation GetFunction(char mode) {
    switch (mode) {
        case SOLID_MODE:
            return DoCuboidSolid;
        case HOLLOW_MODE:
            return DoCuboidHollow;
        case WALLS_MODE:
            return DoCuboidWalls;
        case WIRE_MODE:
            return DoCuboidWire;
        case CORNERS_MODE:
            return DoCuboidCorners;
        default:
            return DoCuboidSolid;
    };
}

static void ZSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }

    ZArguments* arguments = (ZArguments*)object;
    CuboidOperation Operation = GetFunction(arguments->mode);
    Operation(Min(marks[0], marks[1]), Max(marks[0], marks[1]), arguments->brush);
}

static void CleanResources(void* args) {
	ZArguments* arguments = (ZArguments*) args;
	Brush_Free(arguments->brush);
	free(arguments);
}

static void ShowUsage() {
	Message_Player("Usage: &b/Z [mode] [brush/block]&f.");
}

static bool TryParseArguments(const cc_string* args, int argsCount, ZArguments* out_arguments) {
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
		out_arguments->mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
	} else {
		out_arguments->mode = SOLID_MODE;
	}

	if (hasMode && out_arguments->mode == -1) {
		Message_ShowUnknownMode(&args[0]);
		Message_ShowAvailableModes(modesString, modesCount);
		return false;
	}

	Brush* brush = Brush_CreateEmpty();

	if (hasBlockOrBrush) {
		int brushIndex;

		if (hasMode) {
			brushIndex = 1;
		} else {
			brushIndex = 0;
		}


		// Checks that there are no trailing blocks in the command, e.g. `/Z Stone Air` doesn't make sens.
		bool isBlock = args[brushIndex].buffer[0] != '@';
		if (isBlock && argsCount > (brushIndex + 1)) {
			ShowUsage();
			return false;
		}

		if (!Parse_TryParseBlockOrBrush(&args[brushIndex], argsCount - brushIndex, brush)) {
			return false;
		}

		out_arguments->brush = brush;
		return true;
	} else {
		if (!Brush_TryCreateSolid(Inventory_SelectedBlock, brush)) {
			return false;
		}
		out_arguments->brush = brush;
		return true;
	}
}

static void Z_Command(const cc_string* args, int argsCount) {
	ZArguments* arguments = allocate(1, sizeof(ZArguments));
	
	if (!TryParseArguments(args, argsCount, arguments)) {
		free(arguments);
		return;
	}

    MarkSelection_Make(ZSelectionHandler, 2, arguments, CleanResources);
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

struct ChatCommand ZCommand = {
	"Z",
	Z_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Z [mode] [brush/block]",
        "&fDraws a cuboid between two points.",
		"&fList of modes: &bsolid&f (default), &bhollow&f, &bwalls&f, &bwire&f, &bcorners&f.",
		NULL,
		NULL
	},
	NULL
};

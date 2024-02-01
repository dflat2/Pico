#include <stdlib.h>

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Inventory.h"
#include "ClassiCube/src/World.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "ParsingUtils.h"
#include "DataStructures/IVec3FastQueue.h"

static bool s_Repeat;
static int s_Percentage;
static int s_Size;
static unsigned short s_Block;

static void Smooth_Command(const cc_string* args, int argsCount);
static void SmoothSelectionHandler(IVec3* marks, int count);
static int CountBlocksAround(int x, int y, int z);

struct ChatCommand SmoothCommand = {
	"Smooth",
	Smooth_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Smooth <percentage> <size> <block> +",
        "Smooths the selection region.",
        NULL,
        NULL,
        NULL
	},
	NULL
};

static int CountBlocksAround(int x, int y, int z) {
    int total = 0;

    IVec3 min = { x - s_Size, y - s_Size, z - s_Size };
    IVec3 max = { x + s_Size, y + s_Size, z + s_Size };

    for (int x = min.X; x <= max.X; x++) {
        for (int y = min.Y; y <= max.Y; y++) {
            for (int z = min.Z; z <= max.Z; z++) {
                if (!World_Contains(x, y, z)) {
                    continue;
                }

                if (World_GetBlock(x, y, z) == s_Block) {
                    total++;
                }
            }
        }
    }

    return total;
}

static int CountBlocksAroundXY(int x, int y, const int z) {
    int total = 0;

    int minX = x - s_Size;
    int minY = y - s_Size;
    int maxX = x + s_Size;
    int maxY = y + s_Size;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (!World_Contains(x, y, z)) {
                continue;
            }

            if (World_GetBlock(x, y, z) == s_Block) {
                total++;
            }
        }
    }

    return total;
}

static void SmoothSelectionHandler(IVec3* marks, int count) {
    if (count != 2) {
        return;
    }

    IVec3 min = Min(marks[0], marks[1]);
    IVec3 max = Max(marks[0], marks[1]);
    int threshold = ((s_Size * 2) + 1) * ((s_Size * 2) + 1) * ((s_Size * 2) + 1) * s_Percentage / 100;

    IVec3FastQueue* shouldAdd = IVec3FastQueue_CreateEmpty();
    IVec3FastQueue* shouldRemove = IVec3FastQueue_CreateEmpty();
    int blocksAround;

    for (int x = min.X; x <= max.X; x++) {
        for (int y = min.Y; y <= max.Y; y++) {
            blocksAround = CountBlocksAround(x, y, min.Z);

            for (int z = min.Z; z <= max.Z; z++) {
                if (World_GetBlock(x, y, z) == BLOCK_AIR && blocksAround > threshold) {
                    IVec3 vector = { x, y, z };
                    IVec3FastQueue_TryEnqueue(shouldAdd, vector);
                } else if (World_GetBlock(x, y, z) == s_Block && blocksAround <= threshold)  {
                    IVec3 vector = { x, y, z };
                    IVec3FastQueue_TryEnqueue(shouldRemove, vector);
                }

                blocksAround = blocksAround - CountBlocksAroundXY(x, y, z - s_Size) + CountBlocksAroundXY(x, y, z + s_Size + 1);
            }
        }
    }

	Draw_Start("Smooth");
    IVec3 current;

    while (!IVec3FastQueue_IsEmpty(shouldAdd)) {
        current = IVec3FastQueue_Dequeue(shouldAdd);
        Draw_Block(current.X, current.Y, current.Z, s_Block);
    }

    while (!IVec3FastQueue_IsEmpty(shouldRemove)) {
        current = IVec3FastQueue_Dequeue(shouldRemove);
        Draw_Block(current.X, current.Y, current.Z, BLOCK_AIR);
    }

    int blocksAffected = Draw_End();

	if (s_Repeat) {
		MarkSelection_Make(SmoothSelectionHandler, 2, "Smooth");
        return;
	}

    Message_BlocksAffected(blocksAffected);
}

static void Smooth_Command(const cc_string* args, int argsCount) {
	s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount != 3) {
        Message_CommandUsage(SmoothCommand);
        return;
    }

    if (!Parse_TryParseNumber(&args[0], &s_Percentage)) {
        return;
    }

    if (!Parse_TryParseNumber(&args[1], &s_Size)) {
        return;
    }

    if (!Parse_TryParseBlock(&args[2], &s_Block)) {
        return;
    }

	if (s_Repeat) {
		Message_Player("Now repeating &bSmooth&f.");
	}

    MarkSelection_Make(SmoothSelectionHandler, 2, "Smooth");
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

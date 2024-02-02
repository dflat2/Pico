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
static unsigned short s_Block;
static const int PERCENTAGE_THRESHOLD = 50;
static const int SIZE = 2;

// ie. when clicking a block, the volume ((BRUSH_SIZE + 1) * 2) ^ 3 around the block will be affected.
static const int BRUSH_SIZE = 7;

static void Smooth_Command(const cc_string* args, int argsCount);
static void SmoothSelectionHandler(IVec3* marks, int count);
static int CountBlocksAround(int x, int y, int z);

struct ChatCommand SmoothCommand = {
	"Smooth",
	Smooth_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Smooth <block> +",
        "Smooths structures made of &bblock &faround clicked block.",
        NULL,
        NULL,
        NULL
	},
	NULL
};

static int CountBlocksAround(int x, int y, int z) {
    int total = 0;

    IVec3 min = { x - SIZE, y - SIZE, z - SIZE };
    IVec3 max = { x + SIZE, y + SIZE, z + SIZE };

    for (int x_iter = min.X; x_iter <= max.X; x_iter++) {
        for (int y_iter = min.Y; y_iter <= max.Y; y_iter++) {
            for (int z_iter = min.Z; z_iter <= max.Z; z_iter++) {
                if (!World_Contains(x_iter, y_iter, z_iter)) {
                    continue;
                }

                if (World_GetBlock(x_iter, y_iter, z_iter) == s_Block) {
                    total++;
                }
            }
        }
    }

    return total;
}

static int CountBlocksAroundXY(int x, int y, const int z) {
    int total = 0;

    int minX = x - SIZE;
    int minY = y - SIZE;
    int maxX = x + SIZE;
    int maxY = y + SIZE;

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
    const int THRESHOLD = ((SIZE * 2) + 1) * ((SIZE * 2) + 1) * ((SIZE * 2) + 1) * PERCENTAGE_THRESHOLD / 100;

    int minX = marks[0].X - BRUSH_SIZE;
    int minY = marks[0].Y - BRUSH_SIZE;
    int minZ = marks[0].Z - BRUSH_SIZE;
    int maxX = marks[0].X + BRUSH_SIZE;
    int maxY = marks[0].Y + BRUSH_SIZE;
    int maxZ = marks[0].Z + BRUSH_SIZE;

    IVec3FastQueue* shouldAdd = IVec3FastQueue_CreateEmpty();
    IVec3FastQueue* shouldRemove = IVec3FastQueue_CreateEmpty();
    int blocksAround;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // Optimization: (1) only count blocks in volume once per horizontal column.
            blocksAround = CountBlocksAround(x, y, minZ);

            for (int z = minZ; z <= maxZ; z++) {                
                if (World_GetBlock(x, y, z) == BLOCK_AIR && blocksAround > THRESHOLD) {
                    IVec3 vector = { x, y, z };
                    IVec3FastQueue_TryEnqueue(shouldAdd, vector);
                } else if (World_GetBlock(x, y, z) == s_Block && blocksAround <= THRESHOLD)  {
                    IVec3 vector = { x, y, z };
                    IVec3FastQueue_TryEnqueue(shouldRemove, vector);
                }

                // (2) And guess the number of blocks in subsequent volumes by calculating the differences on opposite faces.
                blocksAround = blocksAround - CountBlocksAroundXY(x, y, z - SIZE) + CountBlocksAroundXY(x, y, z + SIZE + 1);
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
        MarkSelection_Make(SmoothSelectionHandler, 1, "Smooth");
        return;
    }

    Message_BlocksAffected(blocksAffected);
}


static void Smooth_Command(const cc_string* args, int argsCount) {
	s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount != 1) {
        Message_CommandUsage(SmoothCommand);
        return;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block)) {
        return;
    }

	if (s_Repeat) {
		Message_Player("Now repeating &bSmooth&f.");
	}

    MarkSelection_Make(SmoothSelectionHandler, 1, "Smooth");
    Message_Player("Place or break a block.");
}

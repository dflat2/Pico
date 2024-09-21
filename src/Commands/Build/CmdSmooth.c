#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Chat.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Memory.h"
#include "Message.h"
#include "Parse.h"
#include "DataStructures/IVec3FastQueue.h"

static const int PERCENTAGE_THRESHOLD = 50;
static unsigned short s_Block;
static int s_Intensity = 2;
static int s_Radius = 5;

static void Smooth_Command(const cc_string* args, int argsCount);

struct ChatCommand SmoothCommand = {
    "Smooth",
    Smooth_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Smooth <block> <radius> [intensity] +",
        "Smooths structures made of &bblock &faround marked coordinates.",
        "\x07 &bblock&f: block name or identifier.",
        "\x07 &bradius&f: size of affected zone (recommended &b5&f)",
        "\x07 &bintensity&f: intensity of the smooth (default &bFloor(radius / 2)&f)."
    },
    NULL
};

static int CountBlocksAround(int x, int y, int z) {
    int total = 0;

    IVec3 min = { x - s_Intensity, y - s_Intensity, z - s_Intensity };
    IVec3 max = { x + s_Intensity, y + s_Intensity, z + s_Intensity };

    for (int x_iter = min.x; x_iter <= max.x; x_iter++) {
        for (int y_iter = min.y; y_iter <= max.y; y_iter++) {
            for (int z_iter = min.z; z_iter <= max.z; z_iter++) {
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

    int minX = x - s_Intensity;
    int minY = y - s_Intensity;
    int maxX = x + s_Intensity;
    int maxY = y + s_Intensity;

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
    const int THRESHOLD = ((s_Intensity * 2) + 1) * ((s_Intensity * 2) + 1) * ((s_Intensity * 2) + 1) * PERCENTAGE_THRESHOLD / 100;

    int minX = marks[0].x - s_Radius;
    int minY = marks[0].y - s_Radius;
    int minZ = marks[0].z - s_Radius;
    int maxX = marks[0].x + s_Radius;
    int maxY = marks[0].y + s_Radius;
    int maxZ = marks[0].z + s_Radius;

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
                    IVec3FastQueue_Enqueue(shouldAdd, vector);
                } else if (World_GetBlock(x, y, z) == s_Block && blocksAround <= THRESHOLD)  {
                    IVec3 vector = { x, y, z };
                    IVec3FastQueue_Enqueue(shouldRemove, vector);
                }

                // (2) And guess the number of blocks in subsequent volumes by calculating the differences on opposite faces.
                blocksAround = blocksAround - CountBlocksAroundXY(x, y, z - s_Intensity) + CountBlocksAroundXY(x, y, z + s_Intensity + 1);
            }
        }
    }

    Draw_Start("Smooth");
    IVec3 current;

    while (!IVec3FastQueue_IsEmpty(shouldAdd)) {
        current = IVec3FastQueue_Dequeue(shouldAdd);
        Draw_Block(current.x, current.y, current.z, s_Block);
    }

    while (!IVec3FastQueue_IsEmpty(shouldRemove)) {
        current = IVec3FastQueue_Dequeue(shouldRemove);
        Draw_Block(current.x, current.y, current.z, BLOCK_AIR);
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break a block.");
        MarkSelection_Make(SmoothSelectionHandler, 1, "Smooth", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    if (argsCount <= 1 || 4 <= argsCount) {
        Message_CommandUsage(SmoothCommand);
        return false;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block)) {
        return false;
    } else if (!Parse_TryParseNumber(&args[1], &s_Radius)) {
        return false;
    } else if (s_Radius <= 1 || 11 <= s_Radius) {
        String_AppendConst(&message, "Parameter &bradius &fshould be between &b2 &fand &b10&f.");
        Chat_Add(&message);
        return false;
    }

    bool hasIntensitySpecified = (argsCount == 3);
    s_Intensity = s_Radius / 2;
    
    if (hasIntensitySpecified) {
        if (!Parse_TryParseNumber(&args[2], &s_Intensity)) {
            return false;
        } else if (s_Intensity == 0) {
            Message_Player("Parameter &bintensity &fcannot be &b0&f.");
            return false;
        } else if (s_Intensity < 0) {
            Message_Player("Parameter &bintensity &fcannot be negative.");
            return false;
        } else if (s_Intensity > s_Radius / 2) {
            Message_Player("Parameter &bintensity &fcannot be larger than half of the &bradius&f.");
            return false;
        }
    }

    return true;
}

static void Smooth_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bSmooth&f.");
    }

    MarkSelection_Make(SmoothSelectionHandler, 1, "Smooth", repeat);
    Message_Selection("&aPlace or break a block.");
}

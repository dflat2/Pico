#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "ParsingUtils.h"
#include "DataStructures/Axis.h"
#include "DataStructures/Array.h"
#include "SPCCommand.h"
#include "Draw.h"

typedef enum SphereMode_ {
	MODE_SOLID = 0,
	MODE_HOLLOW = 1,
} SphereMode;

static SphereMode s_Mode;
static int s_Radius;
static IVec3 s_Center;

static void Sphere_Command(const cc_string* args, int argsCount);
static void DoSphere();
static bool ShouldDraw(int x, int y, int z);
static void ShowUsage();

struct ChatCommand SphereCommand = {
	"Sphere",
	Sphere_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Sphere <radius> [mode] [brush/block]",
        "&fDraws a sphere of radius &b<radius>&f.",
        "&fList of modes: &bsolid&f (default), &bhollow&f.",
        NULL,
		NULL
	},
	NULL
};

SPCCommand SphereSPCCommand = {
	.chatCommand = &SphereCommand,
	.canStatic = true
};

static void DoSphere() {
    Draw_Start("Sphere");

    for (int x = s_Center.X - s_Radius; x <= s_Center.X + s_Radius; x++) {
        for (int y = s_Center.Y - s_Radius; y <= s_Center.Y + s_Radius; y++) {
            for (int z = s_Center.Z - s_Radius; z <= s_Center.Z + s_Radius; z++) {
                if (ShouldDraw(x, y, z)) {
                    Draw_Brush(x, y, z);
                }
            }
        }
    }

    int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static bool ShouldDraw(int x, int y, int z) {
    IVec3 vector = { x, y, z };
    double distance = Distance(vector, s_Center);

    if (s_Mode == MODE_SOLID) {
        return distance <= s_Radius;
    }

    return s_Radius - 1 <= distance && distance <= s_Radius;
}

static void SphereSelectionHandler(IVec3* marks, int count) {
    if (count != 1) {
        return;
    }

    s_Center = marks[0];
    DoSphere();
}

static void ShowUsage() {
	Message_Player("Usage: &b/Sphere <radius> [mode] [brush/block]&f.");
}

static void Sphere_Command(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        ShowUsage();
        return;
    }

    if (!Parse_TryParseNumber(&args[0], &s_Radius)) {
        return;
    } else if (s_Radius <= 0) {
        Message_Player("The &bradius &fmust be positive.");
        return;
    }

    cc_string modesString[] = {
        String_FromConst("solid"),
        String_FromConst("hollow"),
    };

	size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

	bool hasMode = (argsCount >= 2) && Array_ContainsString(&args[1], modesString, modesCount);
    bool hasBlockOrBrush = (argsCount >= 3) || (argsCount == 2) && !hasMode;

	if (hasMode) {
		s_Mode = Array_IndexOfStringCaseless(&args[1], modesString, modesCount);
	} else {
		s_Mode = MODE_SOLID;
	}

    int blockOrBrushIndex = hasMode ? 2 : 1;

    if (hasBlockOrBrush) {
        // Checks that there are no trailing blocks in the command.
        bool isBlock = args[blockOrBrushIndex].buffer[0] != '@';
        if (isBlock && argsCount > blockOrBrushIndex + 1) {
            ShowUsage();
            return;
        }

        if (!Parse_TryParseBlockOrBrush(&args[blockOrBrushIndex], argsCount - blockOrBrushIndex)) {
            return;
        }
	} else {
		Brush_LoadInventory();
	}

    MarkSelection_Make(SphereSelectionHandler, 1);
    Message_Player("&fPlace or break a block to determine the center.");
}

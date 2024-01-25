#include <stdlib.h>
#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "ParsingUtils.h"

static bool s_Repeat = false;

static void Center_Command(const cc_string* args, int argsCount);
static void CenterSelectionHandler(IVec3* marks, int count);
static void Center(IVec3 a, IVec3 b);

struct ChatCommand CenterCommand = {
	"Center",
	Center_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Center [block/brush] +",
		"&fPlaces blocks at the center of your selection.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void Center(IVec3 a, IVec3 b) {
    IVec3 min = Min(a, b);
    IVec3 max = Max(a, b);
    IVec3 delta = { .X = abs(a.X - b.X), .Y = abs(a.Y - b.Y), .Z = abs(a.Z - b.Z) };

    IVec3 centerCuboidMin = {
        .X = (min.X + max.X) / 2,
        .Y = (min.Y + max.Y) / 2,
        .Z = (min.Z + max.Z) / 2
    };

    IVec3 centerCuboidMax = {
        (delta.X % 2) == 0 ? centerCuboidMin.X : (centerCuboidMin.X + 1),
        (delta.Y % 2) == 0 ? centerCuboidMin.Y : (centerCuboidMin.Y + 1),
        (delta.Z % 2) == 0 ? centerCuboidMin.Z : (centerCuboidMin.Z + 1)
    };

	Draw_Start("Center");
    for (int i = centerCuboidMin.X; i <= centerCuboidMax.X; i++) {
        for (int j = centerCuboidMin.Y; j <= centerCuboidMax.Y; j++) {
            for (int k = centerCuboidMin.Z; k <= centerCuboidMax.Z; k++) {
                Draw_Brush(i, j, k);
            }
        }
    }
	int blocksAffected = Draw_End();
    Message_BlocksAffected(blocksAffected);

    char message[128];
    snprintf(message, sizeof(message), "&fDrew cuboid at center from &b(%d, %d, %d)&f to &b(%d, %d, %d)&f.",
             centerCuboidMin.X, centerCuboidMin.Y, centerCuboidMin.Z,
             centerCuboidMax.X, centerCuboidMax.Y, centerCuboidMax.Z);

    Message_Player(message);
}

static void CenterSelectionHandler(IVec3* marks, int count) {
    if (count != 2) {
        return;
    }

    Center(marks[0], marks[1]);

    if (s_Repeat) {
        MarkSelection_Make(CenterSelectionHandler, 2, "Center");
        Message_Player("&fPlace or break two blocks to determine the edges.");
    }
}

static void Center_Command(const cc_string* args, int argsCount) {
    s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);
    
    if (argsCount != 0) {
        if (!Parse_TryParseBlockOrBrush(&args[0], argsCount)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    if (s_Repeat) {
		Message_Player("Now repeating &bCenter&f.");
	}

    MarkSelection_Make(CenterSelectionHandler, 2, "Center");
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

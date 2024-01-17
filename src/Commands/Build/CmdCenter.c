#include <stdlib.h>
#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "VectorsExtension.h"

static void Center_Command(const cc_string* args, int argsCount);
static void CenterSelectionHandler(IVec3* marks, int count);
static void Center(IVec3 a, IVec3 b);

struct ChatCommand CenterCommand = {
	"Center",
	Center_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Center",
		"&fPlaces gold blocks at the center of your selection.",
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
                Draw_Block(i, j, k, BLOCK_GOLD);
            }
        }
    }
	Draw_End();

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
}

static void Center_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("&fUsage: &b/Center");
		MarkSelection_Abort();
		return;
	}

    MarkSelection_Make(CenterSelectionHandler, 2);
    Message_Player("&fPlace or break two blocks to determine the edges.");
}

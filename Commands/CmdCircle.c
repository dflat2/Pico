#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Core.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Inventory.h"

#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "VectorsExtension.h"
#include "ParsingUtils.h"
#include "Axis.h"
#include "SPCCommand.h"
#include "Draw.h"

static Axis s_Axis;
static int s_Radius;
static IVec3 s_Center;

static void Circle_Command(const cc_string* args, int argsCount);
static void MidPointCircleOctant2D(IVec2* out_octant, int* out_count);
static void DoCircle();
static void ShowUsage();

struct ChatCommand CircleCommand = {
	"Circle",
	Circle_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Circle <radius> <axis> [brush/block]",
        "&fDraws a circle of radius &b<radius>&f.",
        "&b<axis> &fmust be &bX&f, &bY&f (default) or &bZ&f.",
        NULL,
		NULL
	},
	NULL
};

SPCCommand CircleSPCCommand = {
	.chatCommand = &CircleCommand,
	.canStatic = true
};

static void MidPointCircleOctant2D(IVec2* out_octant, int* out_count) {
    *out_count = 0;

    IVec2 here = { .X = s_Radius, .Y = 0 };
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

        isMidPointInCircle = (midPointX * midPointX + midPointY * midPointY) <= (s_Radius * s_Radius);

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

static void ReflectOctant(IVec2 octantVector, IVec2* out_reflections) {
    out_reflections[0] = octantVector;
    out_reflections[1].X = octantVector.Y;
    out_reflections[1].Y = octantVector.X;
    out_reflections[2].X = -octantVector.X;
    out_reflections[2].Y = octantVector.Y;
    out_reflections[3].X = octantVector.Y;
    out_reflections[3].Y = -octantVector.X;
    out_reflections[4].X = octantVector.X;
    out_reflections[4].Y = -octantVector.Y;
    out_reflections[5].X = -octantVector.Y;
    out_reflections[5].Y = octantVector.X;
    out_reflections[6].X = -octantVector.X;
    out_reflections[6].Y = -octantVector.Y;
    out_reflections[7].X = -octantVector.Y;
    out_reflections[7].Y = -octantVector.X;
}

static void DoCircle() {
    // `radius` is always an upper bound for the number of blocks in an octant.
    IVec2* octant = allocate(s_Radius, sizeof(IVec2));
    int count = 0;
    MidPointCircleOctant2D(octant, &count);

    IVec3 here;
    IVec2 reflections[8];

    Draw_Start("Circle");

    for (int i = 0; i < count; i++) {
        ReflectOctant(octant[i], reflections);

        for (int j = 0; j < 8; j++) {
            here = Add(s_Center, Transform2DTo3D(reflections[j], s_Axis));
            Draw_Brush(here.X, here.Y, here.Z);
        }
    }

    int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void CircleSelectionHandler(IVec3* marks, int count) {
    if (count != 1) {
        return;
    }

    s_Center = marks[0];
    DoCircle();
}

static void ShowUsage() {
	Message_Player("Usage: &b/Circle <radius> <axis> [brush/block]&f.");
}

static void Circle_Command(const cc_string* args, int argsCount) {
    if (argsCount < 2) {
        ShowUsage();
        return;
    }

    if (!Parse_TryParseNumber(&args[0], &s_Radius)) {
        return;
    }

    if (s_Radius <= 0) {
        Message_Player("The &bradius &fmust be positive.");
        return;
    }

    if (!Parse_TryParseAxis(&args[1], &s_Axis)) {
        return;
    }

    bool hasBlockOrBrush = argsCount >= 3;

    if (hasBlockOrBrush) {
        // Checks that there are no trailing blocks in the command.
        bool isBlock = args[2].buffer[0] != '@';
        if (isBlock && argsCount > 3) {
            ShowUsage();
            return;
        }

        if (!Parse_TryParseBlockOrBrush(&args[2], argsCount - 2)) {
            return;
        }
	} else {
		Brush_LoadInventory();
	}

    MarkSelection_Make(CircleSelectionHandler, 1);
    Message_Player("&fPlace or break a block to determine the center.");
}

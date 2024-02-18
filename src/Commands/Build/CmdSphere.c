#include <stdio.h>

#include "Brushes/Brush.h"
#include "MarkSelection.h"
#include "Message.h"
#include "VectorUtils.h"
#include "Parse.h"
#include "DataStructures/Array.h"
#include "Draw.h"

typedef enum SphereMode_ {
    MODE_SOLID = 0,
    MODE_HOLLOW = 1,
} SphereMode;

static SphereMode s_Mode;
static int s_Radius;
static IVec3 s_Center;

static void Sphere_Command(const cc_string* args, int argsCount);

struct ChatCommand SphereCommand = {
    "Sphere",
    Sphere_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Sphere <radius> [mode] @ +",
        "Draws a sphere.",
        "\x07 &bradius&f: positive non-zero integer.",
        "\x07 &bmode&f: &bsolid&f (default) or &bhollow&f.",
        NULL
    },
    NULL
};

static bool ShouldDraw(int x, int y, int z) {
    IVec3 vector = { x, y, z };
    double distance = VectorUtils_IVec3_Distance(vector, s_Center);

    if (s_Mode == MODE_SOLID) {
        return distance <= s_Radius;
    }

    return s_Radius - 1 <= distance && distance <= s_Radius;
}

static void SphereSelectionHandler(IVec3* marks, int count) {
    s_Center = marks[0];
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

    if (MarkSelection_Repeating()) {
        MarkSelection_Make(SphereSelectionHandler, 1, "Sphere", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static void Sphere_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount == 0) {
        Message_CommandUsage(SphereCommand);
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
    bool hasBlockOrBrush = (argsCount >= 3) || ((argsCount == 2) && !hasMode);

    if (hasMode) {
        s_Mode = Array_IndexOfStringCaseless(&args[1], modesString, modesCount);
    } else {
        s_Mode = MODE_SOLID;
    }

    int blockOrBrushIndex = hasMode ? 2 : 1;

    if (hasBlockOrBrush) {
        if (!Parse_TryParseBlockOrBrush(&args[blockOrBrushIndex], argsCount - blockOrBrushIndex)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    if (repeat) {
        Message_Player("Now repeating &bSphere&f.");
    }

    MarkSelection_Make(SphereSelectionHandler, 1, "Sphere", repeat);
    Message_Player("Place or break a block to determine the center.");
}

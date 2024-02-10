#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Constants.h"

#include "Brushes.h"

static BlockID (*s_CurrentBrushPaint)(int x, int y, int z) = NULL;

BlockID Brush_Paint(int x, int y, int z) {
    return s_CurrentBrushPaint(x, y, z);
}

static Brush* TryFindBrush(const cc_string* name) {
    if (String_CaselessEqualsConst(name, "@Checkered")) {
        return &BrushCheckered;
    } else if (String_CaselessEqualsConst(name, "@Inventory")) {
        return &BrushInventory;
    } else if (String_CaselessEqualsConst(name, "@Perlin")) {
        return &BrushPerlin;
    } else if (String_CaselessEqualsConst(name, "@Rainbow")) {
        return &BrushRainbow;
    } else if (String_CaselessEqualsConst(name, "@Random")) {
        return &BrushRandom;
    } else if (String_CaselessEqualsConst(name, "@Solid")) {
        return &BrushSolid;
    } else if (String_CaselessEqualsConst(name, "@Striped")) {
        return &BrushStriped;
    }

    return NULL;
}

static void ShowCouldNotFindBrush(const cc_string* name) {
    char buffer[STRING_SIZE];
    cc_string message = { buffer, 0, STRING_SIZE };
    String_Format1(&message, "Could not find brush &b%s&f.", name);
    Chat_Add(&message);
}

bool Brush_TryLoad(const cc_string* name, const cc_string* args, int argsCount) {
    Brush* brush = TryFindBrush(name);

    if (brush == NULL) {
        ShowCouldNotFindBrush(name);
        return false;
    }

    s_CurrentBrushPaint = brush->Paint;

    if (!brush->TryParseArguments(args, argsCount)) {
        return false;
    }

    return true;
}

bool Brush_TryLoadSolid(const cc_string* blockName) {
    s_CurrentBrushPaint = BrushSolid.Paint;

    if (!BrushSolid.TryParseArguments(blockName, 1)) {
        return false;
    }

    return true;
}

void Brush_LoadInventory(void) {
    s_CurrentBrushPaint = BrushInventory.Paint;
}

void Brush_Help(const cc_string* name) {
    Brush* brush = TryFindBrush(name);

    if (brush == NULL) {
        ShowCouldNotFindBrush(name);
        return;
    }

    brush->HelpFunction();
}

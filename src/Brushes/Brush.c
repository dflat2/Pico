#include <stdio.h>

#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Chat.h"

#include "Brushes.h"

static void ShowCouldNotFindBrush(const cc_string* name);

static BlockID (*s_CurrentBrushPaint)(int x, int y, int z) = NULL;

BlockID Brush_Paint(int x, int y, int z) {
	return s_CurrentBrushPaint(x, y, z);
}

bool Brush_TryLoad(const cc_string* name, const cc_string* args, int argsCount) {
	Brush* brush = NULL;

	if (String_CaselessEqualsConst(name, "@Checkered")) {
		brush = &BrushCheckered;
	} else if (String_CaselessEqualsConst(name, "@Inventory")) {
		brush = &BrushInventory;
	} else if (String_CaselessEqualsConst(name, "@Rainbow")) {
		brush = &BrushRainbow;
	} else if (String_CaselessEqualsConst(name, "@Random")) {
		brush = &BrushRandom;
	} else if (String_CaselessEqualsConst(name, "@Solid")) {
		brush = &BrushSolid;
	} else if (String_CaselessEqualsConst(name, "@Striped")) {
		brush = &BrushStriped;
	} else {
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

static void ShowCouldNotFindBrush(const cc_string* name) {
	char buffer[64];
	cc_string message = { buffer, 0, 64 };
	String_Format1(&message, "Could not find brush &b%s&f.", name);
	Chat_Add(&message);
}

#include <stdio.h>

#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Chat.h"

#include "Brushes.h"
#include "DataStructures/List.h"

static void FillListBrushes();
static bool TryFindBrush(const cc_string* name, Brush** brush);
static void ShowCouldNotFindBrush(const cc_string* name);

static List* s_ListBrushes = NULL;
static BlockID (*s_CurrentBrushPaint)(int x, int y, int z) = NULL;

BlockID Brush_Paint(int x, int y, int z) {
	return s_CurrentBrushPaint(x, y, z);
}

bool Brush_TryLoad(const cc_string* name, const cc_string* args, int argsCount) {
	if (s_ListBrushes == NULL) {
		FillListBrushes();
	}

	Brush* brush = NULL;

	if (!TryFindBrush(name, &brush)) {
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

void Brush_LoadInventory() {
	s_CurrentBrushPaint = BrushInventory.Paint;
}

static void FillListBrushes() {
	s_ListBrushes = List_CreateEmpty();
	List_Append(s_ListBrushes, &BrushCheckered);
	List_Append(s_ListBrushes, &BrushRainbow);
	List_Append(s_ListBrushes, &BrushSolid);
	List_Append(s_ListBrushes, &BrushInventory);
}

static bool TryFindBrush(const cc_string* name, Brush** builder) {
	int count = List_Count(s_ListBrushes);
	Brush* current;
	cc_string currentName;

	for (int i = 0; i < count; i++) {
		current = List_Get(s_ListBrushes, i);
		currentName = String_FromReadonly(current->name);

		if (String_CaselessEquals(name, &currentName)) {
			*builder = current;
			return true;
		}
	}

	return false;
}

static void ShowCouldNotFindBrush(const cc_string* name) {
	char buffer[64];
	cc_string message = { buffer, 0, 64 };
	String_Format1(&message, "Could not find brush &b%s&f.", name);
	Chat_Add(&message);
}

#include <stdio.h>

#include "CC_API/String.h"
#include "CC_API/Chat.h"

#include "Brushes.h"
#include "List.h"
#include "MemoryAllocation.h"

static List* s_list = NULL;

static void FillList();
static bool TryFindBrushBuilder(const cc_string* name, BrushBuilder** builder);
static void ShowCouldNotFindBrush(const cc_string* name);
static bool TryBuildBrush(BrushBuilder* builder, const cc_string* arguments, int argumentsCount, Brush* brush);

typedef void (*ResourceCleaner)(void* allocatedResource);

typedef struct Brush_ {
	BlockID (*BrushFunc)(int x, int y, int z, void* object);
	void* arguments;
	ResourceCleaner Clean;
} Brush;

BlockID Brush_Paint(Brush* brush, int x, int y, int z) {
	return brush->BrushFunc(x, y, z, brush->arguments);
}

void Brush_Free(Brush* brush) {
	if (brush->Clean != NULL) {
		brush->Clean(brush->arguments);
	}
	free(brush);
}

bool Brush_TryCreate(const cc_string* name, const cc_string* args, int argsCount, Brush* out_brush) {
	if (s_list == NULL) {
		FillList();
	}

	BrushBuilder* builder = NULL;

	if (!TryFindBrushBuilder(name, &builder)) {
		ShowCouldNotFindBrush(name);
		return false;
	}

	if (!TryBuildBrush(builder, args, argsCount, out_brush)) {
		return false;
	}

	return true;
}

bool Brush_TryCreateNormal(BlockID block, bool useInventory, Brush* out_brush) {
	if (s_list == NULL) {
		FillList();
	}

	if (useInventory) {
		cc_string solid = String_FromReadonly("@Solid");
		return Brush_TryCreate(&solid, NULL, 0, out_brush);
	}

	int intBlock = (int)block;
	cc_string solid = String_FromReadonly("@Solid");
	char buffer[64];
	cc_string blockString = String_FromArray(buffer);
	String_Format1(&blockString, "%i", &intBlock);
	return Brush_TryCreate(&solid, &blockString, 1, out_brush);
}

Brush* Brush_CreateEmpty() {
	Brush* brush = allocate(1, sizeof(Brush));
	brush->BrushFunc = NULL;
	brush->arguments = NULL;
	brush->Clean = NULL;
	return brush;
}

static void FillList() {
	s_list = List_CreateEmpty();
	List_Append(s_list, &BrushCheckered_Builder);
	List_Append(s_list, &BrushRainbow_Builder);
	List_Append(s_list, &BrushSolid_Builder);
}

static bool TryFindBrushBuilder(const cc_string* name, BrushBuilder** builder) {
	int count = List_Count(s_list);
	BrushBuilder* current;
	cc_string currentName;

	for (int i = 0; i < count; i++) {
		current = List_Get(s_list, i);
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

static bool TryBuildBrush(BrushBuilder* builder, const cc_string* arguments, int argumentsCount, Brush* brush) {
	void* brushArguments = builder->ArgumentsParser(arguments, argumentsCount);

	if (brushArguments == NULL && builder->Cleaner != NULL) {
		return false;
	}

	brush->BrushFunc = builder->Painter;
	brush->arguments = brushArguments;
	brush->Clean = builder->Cleaner;
	return true;
}

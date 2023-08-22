#ifndef BRUSH_H
#define BRUSH_H

#include <stdbool.h>

#include "CC_API/Core.h"

typedef struct BrushBuilder_ {
	char* name;
	void* (*ArgumentsParser)(const cc_string* args, int argsCount);
	BlockID (*Painter)(int x, int y, int z, void* arguments);
	void (*Cleaner)(void* arguments);
} BrushBuilder;

typedef struct Brush_ Brush;

void Brush_Free(Brush* brush);
BlockID Brush_Paint(Brush* brush, int x, int y, int z);
bool Brush_TryCreate(const cc_string* name, const cc_string* args, int argsCount, Brush* out_brush);
bool Brush_TryCreateNormal(BlockID block, bool useInventory, Brush* out_brush);
Brush* Brush_CreateEmpty();

#endif /* BRUSH_H */

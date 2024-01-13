#ifndef BRUSH_H
#define BRUSH_H

#include <stdbool.h>

#include "ClassiCube/src/Core.h"

typedef struct Brush_ {
	char* name;
	bool (*TryParseArguments)(const cc_string* args, int argsCount);
	BlockID (*Paint)(int x, int y, int z);
} Brush;

BlockID Brush_Paint(int x, int y, int z);
bool Brush_TryLoad(const cc_string* name, const cc_string* args, int argsCount);
bool Brush_TryLoadSolid(const cc_string* blockName);
void Brush_LoadInventory();

#endif /* BRUSH_H */

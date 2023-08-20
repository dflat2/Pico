#include <stdlib.h>

#include "CC_API/Core.h"
#include "CC_API/Inventory.h"

#include "ParsingUtils.h"
#include "Messaging.h"
#include "MemoryAllocation.h"
#include "Brush.h"

typedef struct BrushSolidArgs_ {
	BlockID block;
} BrushSolidArgs;

static void* BrushSolid_Parse(const cc_string* args, int argsCount) {
	if (argsCount == 0) {
		PlayerMessage("&b@Solid&f: please provide a block.");
		return NULL;;
	} else if (argsCount >= 2) {
		PlayerMessage("&b@Solid&f: please provide only one block.");
		return NULL;;
	}

	BlockID block;

	if (!TryParseBlock(&args[0], &block)) {
		return NULL;
	}

	BrushSolidArgs* brushSolidArgs = allocate(1, sizeof(BrushSolidArgs));
	brushSolidArgs->block = block;
	return brushSolidArgs;
}

static BlockID BrushSolid_Paint(int x, int y, int z, void* object) {
	BrushSolidArgs* args = (BrushSolidArgs*)object;
	return args->block;
}

static void BrushSolid_Clean(void* object) {
	BrushSolidArgs* args = (BrushSolidArgs*)object;
	free(args);
}

BrushBuilder BrushSolid_Builder = {
	.name = "@Solid",
	.ArgumentsParser = &BrushSolid_Parse,
	.Painter = &BrushSolid_Paint,
	.Cleaner = &BrushSolid_Clean
}; 

#include "CC_API/BlockID.h"
#include "CC_API/Inventory.h"

#include "ParsingUtils.h"
#include "Messaging.h"
#include "MemoryAllocation.h"

typedef struct BrushSolidArgs_ {
	BlockID block;
	bool useHoldingBlock;
} BrushSolidArgs;

static void* BrushSolid_Parse(const cc_string* args, int argsCount);
static BlockID BrushSolid_Paint(int x, int y, int z, void* object);
static void BrushSolid_Clean(void* object);

BrushBuilder BrushSolid_Builder = {
	.name = "@Solid",
	.ArgumentsParser = &BrushSolid_Parse,
	.Painter = &BrushSolid_Paint,
	.Cleaner = &BrushSolid_Clean
}; 

static void* BrushSolid_Parse(const cc_string* args, int argsCount) {
	if (argsCount >= 2) {
		Message_Player("&b@Solid &fusage: &bSolid <block>&f.");
		return NULL;;
	}

	BrushSolidArgs* brushSolidArgs = allocate(1, sizeof(BrushSolidArgs));
	brushSolidArgs->useHoldingBlock = false;

	if (argsCount == 0) {
		brushSolidArgs->block = BLOCK_AIR;
		brushSolidArgs->useHoldingBlock = true;
	} else if (!TryParseBlock(&args[0], &brushSolidArgs->block)) {
		free(brushSolidArgs);
		return NULL;
	}

	return brushSolidArgs;
}

static BlockID BrushSolid_Paint(int x, int y, int z, void* object) {
	BrushSolidArgs* args = (BrushSolidArgs*)object;

	if (args->useHoldingBlock) {
		args->useHoldingBlock = false;
		args->block = Inventory_SelectedBlock;
	}
	
	return args->block;
}

static void BrushSolid_Clean(void* object) {
	BrushSolidArgs* args = (BrushSolidArgs*)object;
	free(args);
}

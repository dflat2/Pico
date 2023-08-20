#include "CC_API/Core.h"

#include "ParsingUtils.h"
#include "Messaging.h"
#include "MemoryAllocation.h"

typedef struct BrushCheckeredArgs_ {
	BlockID block1;
	BlockID block2;
} BrushCheckeredArgs;

static void* BrushCheckered_Parse(const cc_string* args, int argsCount) {
	if (argsCount < 2) {
		PlayerMessage("&b@Checkered&f: please provide two blocks for the checkers.");
		return NULL;
	} else if (argsCount >= 3) {
		PlayerMessage("&b@Checkered&f: only two blocks are required.");
		return NULL;
	}

	BlockID block1;
	BlockID block2;

	if (!TryParseBlock(&args[0], &block1) || !TryParseBlock(&args[1], &block2)) {
		return NULL;
	}

	BrushCheckeredArgs* brushCheckeredArgs = allocate(1, sizeof(BrushCheckeredArgs));
	brushCheckeredArgs-> block1 = block1;
	brushCheckeredArgs-> block2 = block2;
	return (void*)brushCheckeredArgs;
}

static BlockID BrushCheckered_Paint(int x, int y, int z, void* object) {
	BrushCheckeredArgs* args = (BrushCheckeredArgs*)object;

	if ((x + y + z) % 2 == 0) {
		return args->block1;
	}

	return args->block2;
}

static void BrushCheckered_Clean(void* object) {
	BrushCheckeredArgs* args = (BrushCheckeredArgs*)object;
	free(args);
}

BrushBuilder BrushCheckered_Builder = {
	.name = "@Checkered",
	.ArgumentsParser = &BrushCheckered_Parse,
	.Painter = &BrushCheckered_Paint,
	.Cleaner = &BrushCheckered_Clean
}; 

#include "CC_API/BlockID.h"

#include "Messaging.h"
#include "Brush.h"

static void* BrushRainbow_Parse(const cc_string* args, int argsCount);
static BlockID BrushRainbow_Paint(int x, int y, int z, void* object);

BrushBuilder BrushRainbow_Builder = {
	.name = "@Rainbow",
	.ArgumentsParser = &BrushRainbow_Parse,
	.Painter = &BrushRainbow_Paint,
	.Cleaner = NULL
}; 

static const int s_blocksCount = BLOCK_PINK - BLOCK_RED + 1;

static void* BrushRainbow_Parse(const cc_string* args, int argsCount) {
	if (argsCount > 0) {
		Message_Player("&b@Rainbow&f: no arguments required.");
		return NULL;
	}

	return NULL;
}

static BlockID BrushRainbow_Paint(int x, int y, int z, void* object) {
	return BLOCK_RED + ((x + y + z) % s_blocksCount);
}

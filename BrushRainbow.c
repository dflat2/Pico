#include "ClassiCube/src/BlockID.h"

#include "Messaging.h"
#include "Brush.h"

static bool BrushRainbow_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushRainbow_Paint(int x, int y, int z);

Brush BrushRainbow = {
	.name = "@Rainbow",
	.TryParseArguments = &BrushRainbow_TryParseArguments,
	.Paint = &BrushRainbow_Paint,
}; 

static const int s_BlocksCount = BLOCK_PINK - BLOCK_RED + 1;

static bool BrushRainbow_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount > 0) {
		Message_Player("&b@Rainbow&f: no arguments required.");
		return false;
	}

	return true;
}

static BlockID BrushRainbow_Paint(int x, int y, int z) {
	return BLOCK_RED + ((x + y + z) % s_BlocksCount);
}

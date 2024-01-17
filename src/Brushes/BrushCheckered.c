#include "ParsingUtils.h"
#include "Messaging.h"

static BlockID BrushCheckered_Paint(int x, int y, int z);
static bool BrushCheckered_TryParseArguments(const cc_string* args, int argsCount);

Brush BrushCheckered = {
	.name = "@Checkered",
	.TryParseArguments = &BrushCheckered_TryParseArguments,
	.Paint = &BrushCheckered_Paint,
}; 

static BlockID s_Block1;
static BlockID s_Block2;

static bool BrushCheckered_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount < 2) {
		Message_Player("&b@Checkered&f: please provide two blocks for the checkers.");
		return false;
	} else if (argsCount >= 3) {
		Message_Player("&b@Checkered&f: only two blocks are required.");
		return false;;
	}

	if (!Parse_TryParseBlock(&args[0], &s_Block1) || !Parse_TryParseBlock(&args[1], &s_Block2)) {
		return false;
	}

	return true;
}

static BlockID BrushCheckered_Paint(int x, int y, int z) {
	if ((x + y + z) % 2 == 0) {
		return s_Block1;
	}

	return s_Block2;
}

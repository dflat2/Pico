#include "ClassiCube/src/BlockID.h"
#include "ClassiCube/src/Inventory.h"

#include "ParsingUtils.h"
#include "Messaging.h"

static bool BrushSolid_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushSolid_Paint(int x, int y, int z);

Brush BrushSolid = {
	.TryParseArguments = &BrushSolid_TryParseArguments,
	.Paint = &BrushSolid_Paint,
}; 

static BlockID s_Block;

static bool BrushSolid_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount >= 2) {
		Message_Player("Usage: &b@Solid <block>&f.");
		return false;
	}

	if (!Parse_TryParseBlock(&args[0], &s_Block)) {
		return false;
	}

	return true;
}

static BlockID BrushSolid_Paint(int x, int y, int z) {
	return s_Block;
}

#include "CC_API/BlockID.h"
#include "CC_API/Inventory.h"

#include "ParsingUtils.h"
#include "Messaging.h"

static bool BrushSolid_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushSolid_Paint(int x, int y, int z);

Brush BrushSolid = {
	.name = "@Solid",
	.TryParseArguments = &BrushSolid_TryParseArguments,
	.Paint = &BrushSolid_Paint,
}; 

static BlockID s_Block;
static bool s_UseHoldingBlock;

static bool BrushSolid_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount >= 2) {
		Message_Player("&b@Solid &fusage: &bSolid <block>&f.");
		return false;
	}

	s_UseHoldingBlock = false;

	if (argsCount == 0) {
		s_UseHoldingBlock = true;
	} else if (!TryParseBlock(&args[0], &s_Block)) {
		return false;
	}

	return true;
}

static BlockID BrushSolid_Paint(int x, int y, int z) {
	if (s_UseHoldingBlock) {
		s_UseHoldingBlock = false;
		s_Block = Inventory_SelectedBlock;
	}
	
	return s_Block;
}

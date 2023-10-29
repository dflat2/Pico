#include "CC_API/BlockID.h"
#include "CC_API/Inventory.h"

#include "ParsingUtils.h"
#include "Messaging.h"

static bool BrushInventory_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushInventory_Paint(int x, int y, int z);

Brush BrushInventory = {
	.name = "@Inventory",
	.TryParseArguments = &BrushInventory_TryParseArguments,
	.Paint = &BrushInventory_Paint,
}; 

static bool BrushInventory_TryParseArguments(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("&b@Inventory: no arguments required.");
		return false;
	}

	return true;
}

static BlockID BrushInventory_Paint(int x, int y, int z) {
	return Inventory_SelectedBlock;
}

#include "ClassiCube/src/BlockID.h"
#include "ClassiCube/src/Inventory.h"

#include "Parse.h"
#include "Messaging.h"

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

static void BrushInventory_Help(void) {
	Message_Player("&b@Inventory");
	Message_Player("Outputs the block you are currently holding. It is the default brush.");
}

Brush BrushInventory = {
	.TryParseArguments = &BrushInventory_TryParseArguments,
	.Paint = &BrushInventory_Paint,
	.HelpFunction = &BrushInventory_Help
}; 

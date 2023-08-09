#include <stdlib.h>
#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Game.h"
#include "CC_API/Block.h"
#include "CC_API/Inventory.h"
#include "CC_API/String.h"

#include "WorldUtils.h"
#include "Messaging.h"

static void Place_Command(const cc_string* args, int argsCount) {
	if (argsCount == 2 || argsCount >= 5) {
		PlayerMessage("&fUsage: &b/Place &for &b/Place <block> <x> <y> <z>&f.");
		return;
	}

	BlockID block = Inventory_SelectedBlock;
	IVec3 position = SnapToWorldBoundaries(GetCurrentPlayerPosition());

	bool specifiedBlock = (argsCount == 1) || (argsCount == 4);
	bool specifiedCoords = (argsCount == 3) || (argsCount == 4);

	if (specifiedBlock) {
		int blockInt = Block_Parse(&args[0]);

		if (blockInt == -1) {
			char message[64];
			cc_string ccMessage = { message, 0, 64 };
			String_Format1(&ccMessage, "&fThere is no block &b%s&f.", (void*)&args[0]);
			Chat_Add(&ccMessage);
			return;
		}

		block = (BlockID)blockInt;
	}

	if (specifiedCoords) {
		int start = argsCount == 3 ? 0 : 1;

		if (!TryParseCoordinates(&args[start], &position)) {
			return;
		}
	}

	Game_UpdateBlock(position.X, position.Y, position.Z, block);
	cc_string blockName = Block_UNSAFE_GetName(block);
	char message[64];
	cc_string cc_message = { message, 0, sizeof(message) };
	String_Format4(&cc_message, "&b%s &fblock was placed at &b(%i, %i, %i)&f.", &blockName, &position.X, &position.Y, &position.Z);
	Chat_Add(&cc_message);
}

struct ChatCommand PlaceCommand = {
	"Place",
	Place_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Place [block] [x y z]",
		"&fPlaces &b[block] &fat coordinates &b[x y z]&f.",
		"&fPlaces &b[block] &fat your feet if no coordinates are given.",
		NULL,
		NULL
	},
	NULL
};
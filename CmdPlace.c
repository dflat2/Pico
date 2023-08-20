#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"

#include "WorldUtils.h"
#include "Messaging.h"
#include "Draw.h"

static void Place_Command(const cc_string* args, int argsCount);
static void Place(IVec3 position, BlockID block);

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

static void Place(IVec3 position, BlockID block) {
	cc_string cc_blockName = Block_UNSAFE_GetName(block);
	char blockName[64];
	String_CopyToRaw(blockName, sizeof(blockName), &cc_blockName);
	blockName[63] = '\0';
	char description[64];
	snprintf(description, sizeof(description), "Place %s", blockName);
	
	Draw_Start(description);
	Draw_Block(position.X, position.Y, position.Z, block);
	int blocksAffected = Draw_End();

	char message[64];

	if (blocksAffected == 1) {
		snprintf(message, sizeof(message), "&b%s &fblock was placed at &b(%d, %d, %d)&f.", blockName, position.X, position.Y, position.Z);
	} else {
		snprintf(message, sizeof(message), "&b(%d, %d, %d)&f is already &b%s&f. No block affected.", position.X, position.Y, position.Z, blockName);
	}

	Message_Player(message);
}

static void Place_Command(const cc_string* args, int argsCount) {
	if (argsCount == 2 || argsCount >= 5) {
		Message_Player("&fUsage: &b/Place &for &b/Place <block> <x> <y> <z>&f.");
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

	Place(position, block);
}

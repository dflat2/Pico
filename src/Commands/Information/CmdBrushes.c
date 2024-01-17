#include "ClassiCube/src/Chat.h"

#include "Messaging.h"

static void Brushes_Command(const cc_string* args, int argsCount);

struct ChatCommand BrushesCommand = {
	"Brushes",
	Brushes_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY, {
		"&b/Brushes&f - Lists available brushes.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void Brushes_Command(const cc_string* args, int argsCount) {
	Message_Player("Available brushes:");
	Message_Player(" &b@Solid <block>");
	Message_Player(" &b@Inventory");
	Message_Player(" &b@Checkered <block1> <block2>");
	Message_Player(" &b@Rainbow");
}

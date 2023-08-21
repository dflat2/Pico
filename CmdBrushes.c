#include "CC_API/Chat.h"

#include "Messaging.h"
#include "SPCCommand.h"

static void Brushes_Command(const cc_string* args, int argsCount);

static struct ChatCommand BrushesCommand = {
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

SPCCommand BrushesSPCCommand = {
	.chatCommand = &BrushesCommand,
	.canStatic = false
};


static void Brushes_Command(const cc_string* args, int argsCount) {
	Message_Player("Available brushes:");
	Message_Player(" &b@Checkered <block1> <block2>");
	Message_Player(" &b@Rainbow");
	Message_Player(" &b@Solid <block>");
}

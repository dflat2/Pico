#include <stdio.h>

#include "CC_API/Chat.h"

#include "MarkSelection.h"
#include "Messaging.h"
#include "SPCCommand.h"

static void Paint_Command(const cc_string* args, int argsCount);

static struct ChatCommand PaintCommand = {
	"Paint",
	Paint_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Paint &f- Turns paint mode on/off.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand PaintSPCCommand = {
	.chatCommand = &PaintCommand,
	.canStatic = false
};

static void Paint_Command(const cc_string* args, int argsCount) {
	if (argsCount >= 1) {
		Message_Player("Usage: &b/Paint&f.");
		return;
	}

	MarkSelection_TogglePaint();
}

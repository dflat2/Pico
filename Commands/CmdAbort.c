#include "CC_API/Chat.h"

#include "MarkSelection.h"
#include "Messaging.h"
#include "SPCCommand.h"

static void Abort_Command(const cc_string* args, int argsCount) {
    MarkSelection_Abort();
    Message_Player("&fEvery toggle or action was aborted.");
}

static struct ChatCommand AbortCommand = {
	"Abort",
	Abort_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Abort",
		"&fCancels all toggles and actions.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand AbortSPCCommand = {
	.chatCommand = &AbortCommand,
	.canStatic = false
};

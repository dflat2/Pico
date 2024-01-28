#include "ClassiCube/src/Chat.h"

#include "MarkSelection.h"
#include "Messaging.h"

static void Abort_Command(const cc_string* args, int argsCount);

struct ChatCommand AbortCommand = {
	"Abort",
	Abort_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Abort",
		"&fCancels current selection.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void Abort_Command(const cc_string* args, int argsCount) {
	if (argsCount > 0) {
		Message_CommandUsage(AbortCommand);
	}

	if (MarkSelection_RemainingMarks() == 0) {
		Message_Player("There is nothing to abort.");
		return;
	}

    MarkSelection_Abort();
    Message_Player("&fCurrent selection was aborted.");
}

#include "Messaging.h"
#include "SPCCommand.h"
#include "UndoTree.h"
#include "ParsingUtils.h"

static void Earlier_Command(const cc_string* args, int argsCount);

static struct ChatCommand EarlierCommand = {
	"Earlier",
	Earlier_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Earlier <duration>",
		"Navigates the &bUndoTree &fin the past.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand EarlierSPCCommand = {
	.chatCommand = &EarlierCommand,
	.canStatic = false
};

static void Earlier_Command(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_Player("Usage: &b/Earlier <duration>&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/Earlier");
		return;
	}

	int duration_Second;

	if (!Parse_DeltaTime_Second(&(args[0]), &duration_Second)) {
		Parse_ShowExamplesDeltaTime();
		return;
	}

	int commit;
	if (!UndoTree_Earlier(duration_Second, &commit)) {
		Message_Player("Already at the earliest moment.");
		return;
	} 

	int timestamp = (int)UndoTree_CurrentTimestamp();
	Message_UndoCheckedOut(commit, timestamp);
}
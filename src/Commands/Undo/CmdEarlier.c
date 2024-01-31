#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"
#include "ParsingUtils.h"

static void Earlier_Command(const cc_string* args, int argsCount);

struct ChatCommand EarlierCommand = {
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

static void Earlier_Command(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_CommandUsage(EarlierCommand);
		return;
	}

	int duration_Second;

	if (!Parse_TryParseDeltaTime_Second(&(args[0]), &duration_Second)) {
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

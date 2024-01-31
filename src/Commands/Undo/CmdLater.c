#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"
#include "Format.h"
#include "ParsingUtils.h"

static void Later_Command(const cc_string* args, int argsCount);

struct ChatCommand LaterCommand = {
	"Later",
	Later_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Later <duration>",
		"Navigates the &bUndoTree &fin the future.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void Later_Command(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_CommandUsage(LaterCommand);
		return;
	}

	int duration_Second;

	if (!Parse_TryParseDeltaTime_Second(&(args[0]), &duration_Second)) {
		Parse_ShowExamplesDeltaTime();
		return;
	}

	int commit;
	if (!UndoTree_Later(duration_Second, &commit)) {
		Message_Player("No operation to checkout");
		return;
	} 

	int timestamp = (int)UndoTree_CurrentTimestamp();
	Message_UndoCheckedOut(commit, timestamp);
}

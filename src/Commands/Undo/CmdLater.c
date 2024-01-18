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

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/Later");
		return;
	}

	int duration_Second;

	if (!Parse_TryParseDeltaTime_Second(&(args[0]), &duration_Second)) {
		Parse_ShowExamplesDeltaTime();
		return;
	}

	int commit;
	if (!UndoTree_Later(duration_Second, &commit)) {
		char noOpMsg[64];
		char fromString[] = "00:00:00";
		Format_HHMMSS(UndoTree_CurrentTimestamp(), fromString, sizeof(fromString));
		char maxString[] = "00:00:00";
		Format_HHMMSS(UndoTree_CurrentTimestamp() + duration_Second, maxString, sizeof(maxString));
		snprintf(noOpMsg, sizeof(noOpMsg), "No operation to checkout between &b%s&f and &b%s&f.", fromString, maxString); 
		Message_Player(noOpMsg);
		return;
	} 

	int timestamp = (int)UndoTree_CurrentTimestamp();
	Message_UndoCheckedOut(commit, timestamp);
}
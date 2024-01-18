#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"

static void Undo_Command(const cc_string* args, int argsCount);

struct ChatCommand UndoCommand = {
	"Undo",
	Undo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Undo",
		"Undoes the last operation.",
		"Note: you cannot undo physics.",
		NULL,
		NULL,
	},
	NULL
};

static void Undo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_CommandUsage(UndoCommand);
		Message_Player("If you want to go back in time, use &b/Earlier <duration>&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/Undo");
		return;
	}

	if (!UndoTree_Ascend()) {
		Message_Player("There is nothing to undo.");
		return;
	}

	Message_Player("Undo performed.");
}
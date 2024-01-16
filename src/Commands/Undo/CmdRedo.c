#include "Messaging.h"
#include "SPCCommand.h"
#include "UndoTree.h"

static void Redo_Command(const cc_string* args, int argsCount);

static struct ChatCommand RedoCommand = {
	"Redo",
	Redo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Redo &f- Reverts last &b/Undo&f.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand RedoSPCCommand = {
	.chatCommand = &RedoCommand,
	.canStatic = false
};

static void Redo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("Usage: &b/Redo&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/Redo");
		return;
	}

	if (!UndoTree_Redo()) {
		Message_Player("You have nothing to redo.");
		return;
	}

	Message_Player("Redo performed.");
}
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"

static void Undo_Command(const cc_string* args, int argsCount) {
	if (!UndoTree_Enabled()) {
		PlayerMessage("Cannot &b/Undo&f as the undo system is disabled.");
		PlayerMessage("It can be enabled with &b/Configure UndoTreeEnabled:True&f.");
		return;
	}

	if (!UndoTree_Ascend()) {
		PlayerMessage("There is nothing to undo.");
		return;
	}

	PlayerMessage("Undo performed.");
}

struct ChatCommand UndoCommand = {
	"Undo",
	Undo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Undo &f- Undoes the last operation.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

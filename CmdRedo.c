#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"

static void Redo_Command(const cc_string* args, int argsCount) {
	if (!UndoTree_Enabled()) {
		PlayerMessage("Cannot &b/Redo&f as the undo system is disabled.");
		PlayerMessage("It can be enabled with &b/Configure UndoTreeEnabled:True&f.");
		return;
	}

	if (UndoTree_Redo()) {
		PlayerMessage("Redo performed.");
	} else {
		PlayerMessage("You have nothing to redo.");
	}
}

struct ChatCommand RedoCommand = {
	"Redo",
	Redo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Redo &f- Reverts last movement on the undo tree.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

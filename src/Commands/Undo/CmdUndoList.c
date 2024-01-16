#include "Messaging.h"
#include "SPCCommand.h"
#include "UndoTree.h"

static void UndoList_Command(const cc_string* args, int argsCount);

static struct ChatCommand UndoListCommand = {
	"UndoList",
	UndoList_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/UndoList &f- Lists the terminal nodes of the undo tree.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand UndoListSPCCommand = {
	.chatCommand = &UndoListCommand,
	.canStatic = false
};

static void UndoList_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("Usage: &b/UndoList&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/UndoList");
		return;
	}

	Message_Player("Terminal nodes in &bUndoTree&f:");
	UndoTree_ShowLeaves();
}
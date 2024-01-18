#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"

static void UndoList_Command(const cc_string* args, int argsCount);

struct ChatCommand UndoListCommand = {
	"UndoList",
	UndoList_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/UndoList",
		"Lists the terminal nodes of the undo tree.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void UndoList_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_CommandUsage(UndoListCommand);
		return;
	}

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/UndoList");
		return;
	}

	Message_Player("Terminal nodes in &bUndoTree&f:");
	UndoTree_ShowLeaves();
}
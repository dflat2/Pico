#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"
#include "ParsingUtils.h"
#include "MemoryAllocation.h"
#include "Format.h"
#include "SPCCommand.h"

static void UndoList_Command(const cc_string* args, int argsCount);
static void UndoTree_Command(const cc_string* args, int argsCount);
static void Checkout_SubCommand(const cc_string* args, int argsCount);
static void Later_SubCommand(const cc_string* args, int argsCount);
static void Earlier_SubCommand(const cc_string* args, int argsCount);
static void Redo_SubCommand(const cc_string* args, int argsCount);
static void Descend_SubCommand(const cc_string* args, int argsCount);
static void Ascend_SubCommand(const cc_string* args, int argsCount);
static void UndoTreeShowUsages();
static void ShowCheckedOut(int commit, int timestamp);
static void Redo_Command(const cc_string* args, int argsCount);
static void Undo_Command(const cc_string* args, int argsCount);
static void ShowUndoDisabled(const char* action);

static struct ChatCommand RedoCommand = {
	"Redo",
	Redo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Redo &f- Reverts last movement on the undo tree.",
		"This command does the same thing as &b/UndoTree redo&f.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

static struct ChatCommand UndoCommand = {
	"Undo",
	Undo_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Undo &f- Undoes the last operation.",
		"This command does the same thing as &b/UndoTree ascend&f.",
		"Please note that you cannot undo block physics.",
		NULL,
		NULL,
	},
	NULL
};

static struct ChatCommand UndoTreeCommand = {
	"UndoTree",
	UndoTree_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/UndoTree ascend/descend/redo",
		"&b/UndoTree earlier/later <duration>",
		"&b/UndoTree checkout <operation>",
		"Navigates the undo tree. Read the manual for more information.",
		NULL
	},
	NULL
};

static struct ChatCommand UndoListCommand = {
	"UndoList",
	UndoList_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/UndoList &f- Lists the leaves of the undo tree.",
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
SPCCommand UndoSPCCommand = {
	.chatCommand = &UndoCommand,
	.canStatic = false
};

SPCCommand UndoTreeSPCCommand = {
	.chatCommand = &UndoTreeCommand,
	.canStatic = false
};

SPCCommand UndoListSPCCommand = {
	.chatCommand = &UndoListCommand,
	.canStatic = false
};

static void ShowUndoDisabled(const char* action) {
	char cannotDoMsg[64];
	snprintf(cannotDoMsg, sizeof(cannotDoMsg), "Cannot &b%s&f, as the undo system is disabled.", action);
	Message_Player(cannotDoMsg);
	Message_Player("It can be enabled with &b/Configure UndoTreeEnabled:True&f.");
}

static void Undo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("&b/Undo&f doesn't take any argument.");
		Message_Player("If you meant to go back in time, use &b/UndoTree earlier <duration>&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/Undo");
		return;
	}

	if (!UndoTree_Ascend()) {
		Message_Player("There is nothing to undo.");
		return;
	}

	Message_Player("Undo performed.");
}

static void Redo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("&b/Redo&f doesn't take any argument.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/Redo");
		return;
	}

	if (!UndoTree_Redo()) {
		Message_Player("You have nothing to redo.");
		return;
	}

	Message_Player("Redo performed.");
}

static void ShowCheckedOut(int commit, int timestamp) {
	char timestampString[] = "00:00:00";
    Format_HHMMSS(timestamp, timestampString, sizeof(timestampString));
	char successMsg[64];
	snprintf(successMsg, sizeof(successMsg), "Checked out operation &b%d&f [&b%s&f].", commit, timestampString);
	Message_Player(successMsg);
}

static void UndoTreeShowUsages() {
	Message_Player("Usages:");
	Message_Player("&b/UndoTree ascend/descend/redo");
	Message_Player("&b/UndoTree earlier/later <duration>");
	Message_Player("&b/UndoTree checkout <operation>");
}

static void Ascend_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("Usage: &b/UndoTree ascend&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree ascend");
		return;
	}

	if (!UndoTree_Ascend()) {
		Message_Player("Already at the root node.");
		return;
	}

	Message_Player("Ascend performed.");
}

static void Descend_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("Usage: &b/UndoTree descend&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree descend");
		return;
	}

	if (!UndoTree_Descend()) {
		Message_Player("No node to descend to.");
		return;
	}

	Message_Player("Descend performed.");
}

static void Redo_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		Message_Player("Usage: &b/UndoTree redo&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree redo");
		return;
	}

	if (!UndoTree_Redo()) {
		Message_Player("You have nothing to redo.");
		return;
	}

	Message_Player("Redo performed.");
}

static void Earlier_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_Player("Usage: &b/UndoTree earlier <duration>&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree earlier");
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
	ShowCheckedOut(commit, timestamp);
}

static void Later_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_Player("Usage: &b/UndoTree earlier <duration>&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree later");
		return;
	}

	int duration_Second;

	if (!Parse_DeltaTime_Second(&(args[0]), &duration_Second)) {
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
	ShowCheckedOut(commit, timestamp);
}

static void Checkout_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_Player("Usage: &b/UndoTree checkout <operation>&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree checkout");
		return;
	}

	int operation;

	if (!Convert_ParseInt(&(args[0]), &operation) || operation < 0) {
		Message_Player("&b<operation>&f must be a positive integer.");
		return;
	}

	int ascended;
	int descended;

	if (!UndoTree_Checkout(operation, &ascended, &descended)) {
		char operationNotFoundMsg[64];
		snprintf(operationNotFoundMsg, sizeof(operationNotFoundMsg), "Could not find operation &b%d&f.", operation);
		Message_Player(operationNotFoundMsg);
		return;
	}

	char successMsg[64];

	if (ascended == 0 && descended == 0) {
		snprintf(successMsg, sizeof(successMsg), "Already at operation &b%d&f.", operation);
	} else {
		snprintf(successMsg, sizeof(successMsg), "Checked out operation &b%d&f.", operation);
	}

	Message_Player(successMsg);
}

static void UndoTree_Command(const cc_string* args, int argsCount) {
	if (argsCount == 0) {
		UndoTreeShowUsages();
		return;
	}

	NamedCommandFunc modes[] = {
		{ String_FromReadonly("ascend"), &Ascend_SubCommand },
		{ String_FromReadonly("descend"), &Descend_SubCommand },
		{ String_FromReadonly("redo"), &Redo_SubCommand },
		{ String_FromReadonly("earlier"), &Earlier_SubCommand },
		{ String_FromReadonly("later"), &Later_SubCommand },
		{ String_FromReadonly("checkout"), &Checkout_SubCommand },
	};

	CommandFunc* command = (CommandFunc*) allocate(1, sizeof(CommandFunc));

	if (!Parse_CommandFunc(args[0], modes, sizeof(modes), command)) {
		UndoTreeShowUsages();
		return;
	}

	(*command)(&(args[1]), argsCount - 1);
	free(command);
}

static void UndoList_Command(const cc_string* args, int argsCount) {
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoList");
		return;
	}

	if (argsCount != 0) {
		Message_Player("Usage: &b/UndoList&f.");
		return;
	}

	Message_Player("List of leaves in the undo tree:");
	UndoTree_ShowLeaves();
}

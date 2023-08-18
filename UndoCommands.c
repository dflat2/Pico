#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/String.h"

#include "Messaging.h"
#include "UndoTree.h"
#include "ParsingUtils.h"
#include "MemoryAllocation.h"
#include "Format.h"

static void ShowUndoDisabled(const char* action) {
	char cannotDoMsg[64];
	snprintf(cannotDoMsg, sizeof(cannotDoMsg), "Cannot &b%s&f, as the undo system is disabled.", action);
	PlayerMessage(cannotDoMsg);
	PlayerMessage("It can be enabled with &b/Configure UndoTreeEnabled:True&f.");
}

static void Undo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("&b/Undo&f doesn't take any argument.");
		PlayerMessage("If you meant to go back in time, use &b/UndoTree earlier <duration>&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/Undo");
		return;
	}

	if (!UndoTree_Ascend()) {
		PlayerMessage("There is nothing to undo.");
		return;
	}

	PlayerMessage("Undo performed.");
}

static void Redo_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("&b/Redo&f doesn't take any argument.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/Redo");
		return;
	}

	if (!UndoTree_Redo()) {
		PlayerMessage("You have nothing to redo.");
		return;
	}

	PlayerMessage("Redo performed.");
}

static void ShowCheckedOut(int commit, int timestamp) {
	char timestampString[] = "00:00:00";
    Format_HHMMSS(timestamp, timestampString, sizeof(timestampString));
	char successMsg[64];
	snprintf(successMsg, sizeof(successMsg), "Checked out operation &b%d&f [&b%s&f].", commit, timestampString);
	PlayerMessage(successMsg);
}

static void UndoTreeShowUsages() {
	PlayerMessage("Usages:");
	PlayerMessage("&b/UndoTree ascend/descend/redo");
	PlayerMessage("&b/UndoTree earlier/later <duration>");
	PlayerMessage("&b/UndoTree checkout <operation>");
}

static void Ascend_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("Usage: &b/UndoTree ascend&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree ascend");
		return;
	}

	if (!UndoTree_Ascend()) {
		PlayerMessage("Already at the root node.");
		return;
	}

	PlayerMessage("Ascend performed.");
}

static void Descend_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("Usage: &b/UndoTree descend&f.");
		return;
	}
	
	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree descend");
		return;
	}

	if (!UndoTree_Descend()) {
		PlayerMessage("No node to descend to.");
		return;
	}

	PlayerMessage("Descend performed.");
}

static void Redo_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("Usage: &b/UndoTree redo&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree redo");
		return;
	}

	if (!UndoTree_Redo()) {
		PlayerMessage("You have nothing to redo.");
		return;
	}

	PlayerMessage("Redo performed.");
}

static void Earlier_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		PlayerMessage("Usage: &b/UndoTree earlier <duration>&f.");
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
		PlayerMessage("Already at the earliest moment.");
		return;
	} 

	int timestamp = (int)UndoTree_CurrentTimestamp();
	ShowCheckedOut(commit, timestamp);
}

static void Later_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		PlayerMessage("Usage: &b/UndoTree earlier <duration>&f.");
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
		PlayerMessage(noOpMsg);
		return;
	} 

	int timestamp = (int)UndoTree_CurrentTimestamp();
	ShowCheckedOut(commit, timestamp);
}

static void Checkout_SubCommand(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		PlayerMessage("Usage: &b/UndoTree checkout <operation>&f.");
		return;
	}

	if (!UndoTree_Enabled()) {
		ShowUndoDisabled("/UndoTree checkout");
		return;
	}

	int operation;

	if (!Convert_ParseInt(&(args[0]), &operation) || operation < 0) {
		PlayerMessage("&b<operation>&f must be a positive integer.");
		return;
	}

	int ascended;
	int descended;

	if (!UndoTree_Checkout(operation, &ascended, &descended)) {
		char operationNotFoundMsg[64];
		snprintf(operationNotFoundMsg, sizeof(operationNotFoundMsg), "Could not find operation &b%d&f.", operation);
		PlayerMessage(operationNotFoundMsg);
		return;
	}

	char successMsg[64];

	if (ascended == 0 && descended == 0) {
		snprintf(successMsg, sizeof(successMsg), "Already at operation &b%d&f.", operation);
	} else {
		snprintf(successMsg, sizeof(successMsg), "Checked out operation &b%d&f.", operation);
	}

	PlayerMessage(successMsg);
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

struct ChatCommand RedoCommand = {
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

struct ChatCommand UndoCommand = {
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

struct ChatCommand UndoTreeCommand = {
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


#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Messaging.h"
#include "UndoTree.h"

static void Checkout_Command(const cc_string* args, int argsCount);

struct ChatCommand CheckoutCommand = {
	"Checkout",
	Checkout_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Checkout <operation>",
		"Navigates the &bUndoTree &fby providing a specific operation ID.",
		"Use &b/UndoList &fto list terminal node IDs.",
		NULL,
		NULL,
	},
	NULL
};

static void Checkout_Command(const cc_string* args, int argsCount) {
	if (argsCount != 1) {
		Message_CommandUsage(CheckoutCommand);
		return;
	}

	if (!UndoTree_Enabled()) {
		Message_UndoDisabled("/Checkout");
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
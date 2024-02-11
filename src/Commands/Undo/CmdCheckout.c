#include <stdio.h>

#include "ClassiCube/src/Constants.h"

#include "Message.h"
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

    int operation;

    if (!Convert_ParseInt(&(args[0]), &operation) || operation < 0) {
        Message_Player("&b<operation>&f must be a positive integer.");
        return;
    }

    int ascended;
    int descended;

    if (!UndoTree_Checkout_MALLOC(operation, &ascended, &descended)) {
        char operationNotFoundMsg[STRING_SIZE];
        snprintf(operationNotFoundMsg, sizeof(operationNotFoundMsg), "Could not find operation &b%d&f.", operation);
        Message_Player(operationNotFoundMsg);
        return;
    }

    char messageBuffer[STRING_SIZE];
    cc_string message = { messageBuffer, .length = 0, .capacity = STRING_SIZE };
    UndoTree_FormatCurrentNode(&message);
    Chat_AddOf(&message, MSG_TYPE_SMALLANNOUNCEMENT);
}

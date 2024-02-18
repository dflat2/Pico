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
        "Navigates the undo tree by providing a specific operation.",
        "Do &b/UndoList &fto list the latest terminal operations identifiers.",
        "\x07 &boperation&f: positive integer.",
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
        Message_Player("&boperation&f must be a positive integer.");
        return;
    }

    UndoTree_Checkout(operation);
}

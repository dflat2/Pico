#include "Message.h"

static void Brushes_Command(const cc_string* args, int argsCount);

struct ChatCommand BrushesCommand = {
    "Brushes",
    Brushes_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY, {
        "&b/Brushes",
        "Lists available brushes.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Brushes_Command(const cc_string* args, int argsCount) {
    if (argsCount > 0) {
        Message_CommandUsage(BrushesCommand);
        return;
    }

    Message_Player("Available brushes:");
    Message_Player(" &b@Checkered <block1> <block2>");
    Message_Player(" &b@Inventory");
    Message_Player(" &b@Rainbow");
    Message_Player(" &b@Random <block1> <block2>");
    Message_Player(" &b@Solid <block>");
    Message_Player(" &b@Striped <block1> <block2>");
}

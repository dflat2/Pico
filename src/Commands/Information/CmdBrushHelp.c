#include "ClassiCube/src/Chat.h"

#include "Message.h"
#include "Brushes/Brush.h"

static void BrushHelp_Command(const cc_string* args, int argsCount);

struct ChatCommand BrushHelpCommand = {
    "BrushHelp",
    BrushHelp_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY, {
        "&b/BrushHelp <brush>",
        "Displays help for using &bbrush&f.",
        "Do &b/Brushes &ffor a list of all brushes.",
        NULL,
        NULL
    },
    NULL
};

static void BrushHelp_Command(const cc_string* args, int argsCount) {
    if (argsCount != 1) {
        Message_CommandUsage(BrushHelpCommand);
        return;
    }

    Brush_Help(args);
}

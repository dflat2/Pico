#include "ClassiCube/src/Constants.h"

#include "Message.h"
#include "Brushes/Brush.h"

static void HelpBrush_Command(const cc_string* args, int argsCount);

struct ChatCommand HelpBrushCommand = {
    "HelpBrush",
    HelpBrush_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/HelpBrush [brush]",
        "Displays help for using &bbrush &fif given, lists all brushes otherwise.",
        NULL,
        NULL,
        NULL,
    },
    NULL
};

static void HelpBrush_Command(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        Message_Player("Available brushes: &b@Checkered&f, &b@Inventory&f, &b@Perlin&f,");
        Message_Player("&b@Rainbow&f, &b@Random&f, &b@Solid &fand &b@Striped&f.");
        return;
    }

    if (argsCount >= 2) {
        Message_CommandUsage(HelpBrushCommand);
        return;
    }

    char brushStringBuffer[STRING_SIZE];
    cc_string brushString = String_FromArray(brushStringBuffer);
    
    if (args[0].buffer[0] != '@') {
        String_AppendConst(&brushString, "@");
    }

    String_AppendString(&brushString, &args[0]);
    Brush_Help(&brushString);
}

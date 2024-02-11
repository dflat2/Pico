#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Message.h"
#include "Parse.h"

static void PaintSelectionHandler(IVec3* marks, int count) {
    if (count != 1) {
        return;
    }

    Draw_Start("Paint");
    Draw_Brush(marks[0].X, marks[0].Y, marks[0].Z);
    Draw_End();

    MarkSelection_Make(&PaintSelectionHandler, 1, "Paint");
}

static void Paint_Command(const cc_string* args, int argsCount) {
    if (argsCount >= 1) {
        if (!Parse_TryParseBlockOrBrush(&args[0], argsCount)) {
            return;
        }
    } else {
        Brush_LoadInventory();
    }

    Message_Player("Place or break blocks to paint. Stop with &b/Abort&f.");
    MarkSelection_Make(&PaintSelectionHandler, 1, "Paint");
}

struct ChatCommand PaintCommand = {
    "Paint",
    Paint_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Paint [block/brush]",
        "Paints blocks.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

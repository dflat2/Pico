#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Brushes/Brush.h"
#include "Draw.h"
#include "MarkSelection.h"
#include "Messaging.h"
#include "ParsingUtils.h"

static bool s_Repeat = false;

static void Paint_Command(const cc_string* args, int argsCount);
static void PaintSelectionHandler(IVec3* marks, int count);
static void ShowUsage();

struct ChatCommand PaintCommand = {
	"Paint",
	Paint_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Paint [block/brush] [+]&f- Paints a block.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

static void Paint_Command(const cc_string* args, int argsCount) {
    s_Repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

	if (argsCount >= 1) {
		// Checks that there are no trailing blocks.
		bool isBlock = args[0].buffer[0] != '@';
		if (isBlock && argsCount > 1) {
			ShowUsage();
			return;
		}

		if (!Parse_TryParseBlockOrBrush(&args[0], argsCount)) {
			return;
		}
	} else {
		Brush_LoadInventory();
	}

	if (s_Repeat) {
		Message_Player("Now repeating &bPaint&f.");
	}

	Message_Player("&fPlace or break a block to paint.");
	MarkSelection_Make(&PaintSelectionHandler, 1);
}

static void PaintSelectionHandler(IVec3* marks, int count) {
    if (count != 1) {
        return;
    }

	Draw_Start("Paint");
    Draw_Brush(marks[0].X, marks[0].Y, marks[0].Z);
    int blocksAffected = Draw_End();
    Message_BlocksAffected(blocksAffected);

    if (s_Repeat) {
		Message_Player("&fPlace or break a block to paint.");
		MarkSelection_Make(&PaintSelectionHandler, 1);
    }
}

static void ShowUsage() {
	Message_Player("Usage: &b/Paint [brush/block] [+]&f.");
}
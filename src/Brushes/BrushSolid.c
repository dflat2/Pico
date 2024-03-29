#include "Parse.h"
#include "Message.h"
#include "Brushes/Brush.h"

static BlockID s_Block;

static bool BrushSolid_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount >= 2) {
        Message_Player("Usage: &b@Solid <block>&f.");
        return false;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block)) {
        return false;
    }

    return true;
}

static BlockID BrushSolid_Paint(int x, int y, int z) {
    return s_Block;
}

static void BrushSolid_Help(void) {
    Message_Player("&b@Solid <block>");
    Message_Player("Outputs &bblock&f.");
    Message_Player("\x07 &bblock&f: block name or identifier.");
}

Brush BrushSolid = {
    .TryParseArguments = &BrushSolid_TryParseArguments,
    .Paint = &BrushSolid_Paint,
    .HelpFunction = &BrushSolid_Help,
}; 

#include "Parse.h"
#include "Message.h"
#include "Brushes/Brush.h"

static BlockID s_Block1;
static BlockID s_Block2;

static bool BrushCheckered_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount < 2) {
        Message_Player("&b@Checkered&f: please provide two blocks for the checkers.");
        return false;
    } else if (argsCount >= 3) {
        Message_Player("&b@Checkered&f: only two blocks are required.");
        return false;;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block1) || !Parse_TryParseBlock(&args[1], &s_Block2)) {
        return false;
    }

    return true;
}

static BlockID BrushCheckered_Paint(int x, int y, int z) {
    if ((x + y + z) % 2 == 0) {
        return s_Block1;
    }

    return s_Block2;
}

static void BrushCheckered_Help(void) {
    Message_Player("&b@Checkered <block1> <block2>");
    Message_Player("Alternates between &bblock1 &fand &bblock2&f.");
    Message_Player("\x07 &bblock1&f and &bblock2&f: block names or identifiers.");
}

Brush BrushCheckered = {
    .TryParseArguments = &BrushCheckered_TryParseArguments,
    .Paint = &BrushCheckered_Paint,
    .HelpFunction = &BrushCheckered_Help,
}; 

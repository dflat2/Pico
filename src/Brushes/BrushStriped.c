#include "Parse.h"
#include "Messaging.h"

static BlockID s_Block1;
static BlockID s_Block2;

static bool BrushStriped_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount < 2) {
        Message_Player("&b@Striped&f: please provide two blocks.");
        return false;
    } else if (argsCount >= 3) {
        Message_Player("&b@Striped&f: only two blocks are required.");
        return false;;
    }

    if (!Parse_TryParseBlock(&args[0], &s_Block1) || !Parse_TryParseBlock(&args[1], &s_Block2)) {
        return false;
    }

    return true;
}

static BlockID BrushStriped_Paint(int x, int y, int z) {
    if (((x + y + z) / 2) % 2 == 0) {
        return s_Block1;
    }

    return s_Block2;
}

static void BrushStriped_Help(void) {
    Message_Player("&b@Striped <block1> <block2>");
    Message_Player("Alternates between &bblock1 &fand &bblock2&f diagonally.");
}

Brush BrushStriped = {
    .TryParseArguments = &BrushStriped_TryParseArguments,
    .Paint = &BrushStriped_Paint,
    .HelpFunction = &BrushStriped_Help,
};

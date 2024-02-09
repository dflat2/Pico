#include "ClassiCube/src/BlockID.h"

#include "Message.h"
#include "Brush.h" 

static bool BrushRainbow_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount > 0) {
        Message_Player("&b@Rainbow&f: no arguments required.");
        return false;
    }

    return true;
}

static BlockID BrushRainbow_Paint(int x, int y, int z) {
    const int BLOCKS_COUNT = BLOCK_PINK - BLOCK_RED + 1;
    return BLOCK_RED + ((x + y + z) % BLOCKS_COUNT);
}

static void BrushRainbow_Help(void) {
    Message_Player("&b@Rainbow");
    Message_Player("Draws a rainbow using wool blocks.");
}

Brush BrushRainbow = {
    .TryParseArguments = &BrushRainbow_TryParseArguments,
    .Paint = &BrushRainbow_Paint,
    .HelpFunction = &BrushRainbow_Help
};

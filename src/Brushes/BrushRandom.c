#include <time.h>
#include <stdlib.h>

#include "ClassiCube/src/BlockID.h"

#include "Brushes/Brush.h"
#include "Parse.h"
#include "Message.h"

#define MAX_BLOCKS 10

static bool BrushRandom_TryParseArguments(const cc_string* args, int argsCount);
static BlockID BrushRandom_Paint(int x, int y, int z);
static bool TryParseBlockWeight(const cc_string* argument, BlockID* out_block, int* out_weight);
static void UpdateCumulativeWeights(void);
static void UpdateTotal(void);

static BlockID s_Blocks[MAX_BLOCKS];
static int s_Weights[MAX_BLOCKS];
static int s_CumulativeWeights[MAX_BLOCKS];
static int s_Total = 0;
static int s_Count = 0;
static const BlockID BLOCK_ERROR = BLOCK_GOLD;

static bool BrushRandom_TryParseArguments(const cc_string* args, int argsCount) {
    if (argsCount > MAX_BLOCKS) {
        Message_Player("Cannot use &b@Random &fwith more than ten blocks.");
        return false;
    } else if (argsCount == 0) {
        Message_Player("&b@Random&f: no blocks provided.");
        Message_Player("Example: &b@Random Stone/9 Air/1&f.");
        return false;
    }

    for (int i = 0; i < argsCount; i++) {
        if (!TryParseBlockWeight(&args[i], &s_Blocks[i], &s_Weights[i])) {
            return false;
        }
    }

    s_Count = argsCount;
    UpdateCumulativeWeights();
    UpdateTotal();

    srand(time(NULL));
    return true;
}

static BlockID BrushRandom_Paint(int x, int y, int z) {
    int random = rand() % s_Total;

    for (int i = 0; i < s_Count; i++) {
        if (random < s_CumulativeWeights[i]) {
            return s_Blocks[i];
        }
    }

    return BLOCK_ERROR;
}

static bool TryParseBlockWeight(const cc_string* argument, BlockID* out_block, int* out_weight) {
    cc_string parts[2];
    int partsCount;

    partsCount = String_UNSAFE_Split(argument, '/', parts, 2);

    if (!Parse_TryParseBlock(&parts[0], out_block)) {
        return false;
    }

    if (partsCount == 1) {
        *out_weight = 1;
        return true;
    }

    char buffer[STRING_SIZE];
    cc_string errorMessage = String_FromArray(buffer);

    if (!Convert_ParseInt(&parts[1], out_weight)) {
        String_Format1(&errorMessage, "Could not parse integer &b%s&f.", &parts[0]);
        return false;
    } else if (*out_weight <= 0) {
        String_Format1(&errorMessage, "Invalid &b@Random &fweight: &b%s&f.", &parts[0]);
        return false;
    }

    return true;
}

static void UpdateCumulativeWeights(void) {
    s_CumulativeWeights[0] = s_Weights[0];

    for (int i = 1; i < s_Count; i++) {
        s_CumulativeWeights[i] = s_CumulativeWeights[i - 1] + s_Weights[i];
    }
}

static void UpdateTotal(void) {
    s_Total = 0;

    for (int i = 0; i < s_Count; i++) {
        s_Total += s_Weights[i];
    }
}

static void BrushRandom_Help(void) {
    Message_Player("&b@Random <block1> <block2>");
    Message_Player("Outputs a block randomly between &bblock1 &fand &bblock2&f.");
    Message_Player("You can use more than two blocks. You can also weight blocks,");
    Message_Player("for example &b@Random grass/49 dirt/1 gravel/1&f.");
}

Brush BrushRandom = {
    .TryParseArguments = &BrushRandom_TryParseArguments,
    .Paint = &BrushRandom_Paint,
    .HelpFunction = &BrushRandom_Help
};

#include <stdio.h>
#include <stdlib.h>

#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Constants.h"

#include "Message.h"
#include "MarkSelection.h"
#include "VectorUtils.h"
#include "Parse.h"

static void Measure_Command(const cc_string* args, int argsCount);

struct ChatCommand MeasureCommand = {
    "Measure",
    Measure_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Measure [block] +",
        "Display the dimensions between two points. Counts &bblock &fif given.",
        "\x07 &bblock&f: block name or identifier.",
        NULL,
        NULL
    },
    NULL
};

static BlockID s_Blocks[10];
static int s_Count;

static void ShowCountedBlocks(int* counts) {
    cc_string currentBlockName;
    char buffer[STRING_SIZE];
    cc_string currentMessage = { buffer, 0, STRING_SIZE };

    for (int i = 0; i < s_Count; i++) {
        currentBlockName = Block_UNSAFE_GetName(s_Blocks[i]);
        currentMessage.length = 0;
        String_Format2(&currentMessage, "&b%s&f: &b%i", &currentBlockName, &counts[i]);
        Chat_Add(&currentMessage);
    }
}

static void CountBlocks(int x1, int y1, int z1, int x2, int y2, int z2) {
    int counts[10] = { 0 };
    BlockID currentBlock;

    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                currentBlock = World_GetBlock(x, y, z);

                for (int i = 0; i < s_Count; i++) {
                    if (s_Blocks[i] == currentBlock) {
                        counts[i]++;
                        break;
                    }
                }
            }
        }
    }

    ShowCountedBlocks(counts);
}

static void MeasureSelectionHandler(IVec3* marks, int count) {
    char message[STRING_SIZE];
    snprintf(&message[0], 64, "Measuring from &b(%d, %d, %d)&f to &b(%d, %d, %d)&f.",
            marks[0].X, marks[0].Y, marks[0].Z,
            marks[1].X, marks[1].Y, marks[1].Z);
    Message_Player(&message[0]);

    int width = abs(marks[0].X - marks[1].X) + 1;
    int height = abs(marks[0].Y - marks[1].Y) + 1;
    int length = abs(marks[0].Z - marks[1].Z) + 1;
    int volume = width * height * length;

    snprintf(&message[0], 64, "&b%d &fwide, &b%d &fhigh, &b%d &flong, &b%d &fblocks.", width, height, length, volume);
    Message_Player(&message[0]);

    IVec3 min = VectorUtils_IVec3_Min(marks[0], marks[1]);
    IVec3 max = VectorUtils_IVec3_Max(marks[0], marks[1]);
    
    CountBlocks(min.X, min.Y, min.Z, max.X, max.Y, max.Z);

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break two blocks to determine the edges.");
        MarkSelection_Make(MeasureSelectionHandler, 2, "Measure", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }
}

static void Measure_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (argsCount > 10) {
        Message_Player("Cannot measure more than 10 blocks.");
        return;
    }

    s_Count = argsCount;
    int currentBlock;

    for (int i = 0; i < argsCount; i++) {
        currentBlock = Block_Parse(&args[i]);

        if (currentBlock == -1) {
            Message_ShowUnknownBlock(&args[i]);
            return;
        }

        s_Blocks[i] = (BlockID)currentBlock;
    }

    if (repeat) {
        Message_Player("Now repeating &bMeasure&f.");
    }

    Message_Selection("&aPlace or break two blocks to determine the edges.");
    MarkSelection_Make(MeasureSelectionHandler, 2, "Measure", repeat);
}

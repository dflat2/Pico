#include <stdlib.h>

#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Lighting.h"

#include "Draw.h"
#include "Messaging.h"

static void FixGrass_Command(const cc_string* args, int argsCount);

struct ChatCommand FixGrassCommand = {
    "FixGrass",
    FixGrass_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/FixGrass",
        "Fixes grass and dirt blocks according to the lighting.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void FixGrass_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(FixGrassCommand);
        return;
    }

    BlockID current;
    bool lit;

    Draw_Start("FixGrass");

    for (int x = 0; x < World.Width; x++) {
        for (int y = 0; y < World.Height; y++) {
            for (int z = 0; z < World.Length; z++) {
                current = World_GetBlock(x, y, z);
                lit = Lighting.IsLit(x, y, z);

                if (current == BLOCK_DIRT && lit) {
                    Draw_Block(x, y, z, BLOCK_GRASS);
                } else if (current == BLOCK_GRASS && !lit) {
                    Draw_Block(x, y, z, BLOCK_DIRT);
                }
            }
        }
    }

    int blocksAffected = Draw_End();
    Message_BlocksAffected(blocksAffected);
}

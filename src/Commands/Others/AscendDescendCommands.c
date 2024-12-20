#include <stdbool.h>

#include "ClassiCube/src/Entity.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/World.h"

#include "Message.h"

static void Ascend_Command(const cc_string* args, int argsCount);
static void Descend_Command(const cc_string* args, int argsCount);

struct ChatCommand AscendCommand = {
    "Ascend",
    Ascend_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Ascend",
        "Teleports you to the first free space above you.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

struct ChatCommand DescendCommand = {
    "Descend",
    Descend_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Descend",
        "Teleports you to the first free space below you.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static bool CanPassThrough(BlockID id) {
    enum CollideType collision = Blocks.Collide[id];
    return (collision == COLLIDE_NONE) ||
           (collision == COLLIDE_LIQUID) ||
           (collision == COLLIDE_WATER) ||
           (collision == COLLIDE_LAVA) ||
           (collision == COLLIDE_CLIMB);
}

static bool IsSolidBlock(BlockID id) {
    return !CanPassThrough(id);
}

static bool CanStandOnBlock(int x, int y, int z) {
    BlockID below = World_Contains(x, y - 1, z) ? World_GetBlock(x, y - 1, z) : BLOCK_AIR;
    BlockID feet = World_Contains(x, y, z) ? World_GetBlock(x, y, z) : BLOCK_AIR;
    BlockID head = World_Contains(x, y + 1, z) ? World_GetBlock(x, y + 1, z) : BLOCK_AIR;

    // y == 0 is always solid because it's bedrock, though outside of the world boundaries.
    if (y == 0) {
        return CanPassThrough(feet) && CanPassThrough(head);
    }

    return IsSolidBlock(below) && CanPassThrough(feet) && CanPassThrough(head);
}

static bool TryFindAbove(Vec3 currentPosition, Vec3* ascendPosition) {
    int x = (int) currentPosition.x;
    int y = (int) currentPosition.y;
    int z = (int) currentPosition.z;

    ascendPosition->x = currentPosition.x;
    ascendPosition->z = currentPosition.z;

    for (int yCandidate = y + 1; yCandidate <= World.Height; yCandidate++) {
        if (CanStandOnBlock(x, yCandidate, z)) {
            ascendPosition->y = (float)yCandidate;
            return true;
        }
    }

    return false;
}

static bool TryFindBelow(Vec3 currentPosition, Vec3* descendPosition) {
    int x = (int) currentPosition.x;
    int y = (int) currentPosition.y;
    int z = (int) currentPosition.z;

    descendPosition->x = currentPosition.x;
    descendPosition->z = currentPosition.z;

    for (int yCandidate = y - 1; yCandidate >= 0; yCandidate--) {
        if (CanStandOnBlock(x, yCandidate, z)) {
            descendPosition->y = (float)yCandidate;
            return true;
        }
    }

    return false;
}

static void Ascend_Command(const cc_string* args, int argsCount) {
    if (argsCount >= 1) {
        Message_CommandUsage(AscendCommand);
        return;
    }

    struct LocalPlayer* player = Entities.CurPlayer;
    struct Entity* playerEntity = &player->Base;

    Vec3 currentPosition = playerEntity->Position;
    Vec3 ascendPosition;

    bool success = TryFindAbove(currentPosition, &ascendPosition);

    if (!success) {
        Message_Player("There are no blocks above to ascend to.");
        return;
    }

    struct LocationUpdate update;
    update.flags = LU_HAS_POS;
    update.pos = ascendPosition;

    playerEntity->VTABLE->SetLocation(playerEntity, &update);
}

static void Descend_Command(const cc_string* args, int argsCount) {
    if (argsCount >= 1) {
        Message_CommandUsage(DescendCommand);
        return;
    }

    struct LocalPlayer* player = Entities.CurPlayer;
    struct Entity* playerEntity = &player->Base;

    Vec3 currentPosition = playerEntity->Position;
    Vec3 descendPosition;

    bool success = TryFindBelow(currentPosition, &descendPosition);

    if (!success) {
        Message_Player("There are no blocks below to descend to.");
        return;
    }

    struct LocationUpdate update;
    update.flags = LU_HAS_POS;
    update.pos = descendPosition;

    playerEntity->VTABLE->SetLocation(playerEntity, &update);
}

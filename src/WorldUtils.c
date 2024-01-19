#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/Entity.h"
#include "ClassiCube/src/Chat.h"

IVec3 GetCurrentPlayerPosition() {
	struct LocalPlayer* player = (struct LocalPlayer*)Entities.List[ENTITIES_SELF_ID];
	struct Entity playerEntity = player->Base;
	Vec3 currentPosition = playerEntity.Position;

    IVec3 position;
    position.X = (int)currentPosition.X;
    position.Y = (int)currentPosition.Y;
    position.Z = (int)currentPosition.Z;

    return position;
}

IVec3 SnapToWorldBoundaries(IVec3 coords) {
    if (coords.X < 0) {
        coords.X = 0;
    } else if (coords.X >= World.Width) {
        coords.X = World.Width - 1;
    }

    if (coords.Y < 0) {
        coords.Y = 0;
    } else if (coords.Y >= World.Height) {
        coords.Y = World.Height - 1;
    }

    if (coords.Z < 0) {
        coords.Z = 0;
    } else if (coords.Z >= World.Length) {
        coords.Z = World.Length - 1;
    }

    return coords;
}

bool IsInWorldBoundaries(int x, int y, int z) {
	return (0 <= x && x <= World.Width) &&
		   (0 <= y && y <= World.Height) &&
		   (0 <= z && z <= World.Length);
}

BlockID GetBlock(int x, int y, int z) {
	int i = World_Pack(x, y, z);
	return (BlockID)World_GetRawBlock(i);
}

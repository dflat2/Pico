#include "CC_API/World.h"
#include "CC_API/Block.h"
#include "CC_API/BlockID.h"
#include "CC_API/Entity.h"
#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"

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

static bool TryParseSingleCoordinate(const cc_string* coordinateString, int* result, bool* isRelative) {
    char first = coordinateString->buffer[0];
    char last = coordinateString->buffer[coordinateString->length - 1];
    *isRelative = (first == '(') && (last == ')');

    cc_string number = { coordinateString->buffer, coordinateString->length, coordinateString->capacity };

    if (*isRelative) {
        number.buffer = &(number.buffer[1]);
        number.length -= 2;
    }

    if (Convert_ParseInt(&number, result)) {
        return true;
    }

    return false;
}

static void CoordinateError(const cc_string* coordinate) {
    char error[64];
    cc_string cc_error = { error, 0, 64 };
    String_Format1(&cc_error, "&fCould not parse coordinate &b%s&f.", coordinate);
    Chat_Add(&cc_error);
}

bool TryParseCoordinates(const cc_string* coordinates, IVec3* result) {
	IVec3 playerPosition = GetCurrentPlayerPosition();

    int arrayTarget[3];
    int arrayPlayerPosition[3] = { playerPosition.X, playerPosition.Y, playerPosition.Z };
    bool isRelative;

    for (int i = 0; i < 3; i++) {
        if (!TryParseSingleCoordinate(&coordinates[i], &arrayTarget[i], &isRelative)) {
            CoordinateError(&coordinates[i]);
            return false;
        }

        if (isRelative) {
            arrayTarget[i] += arrayPlayerPosition[i];
        }
    }

    result->X = arrayTarget[0];
	result->Y = arrayTarget[1];
	result->Z = arrayTarget[2];
	return true;
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

bool CanPassThrough(BlockID id) {
	enum CollideType collision = Blocks.Collide[id];
	return (collision == COLLIDE_NONE) ||
		   (collision == COLLIDE_LIQUID) ||
		   (collision == COLLIDE_WATER) ||
		   (collision == COLLIDE_LAVA) ||
		   (collision == COLLIDE_CLIMB);
}

bool IsSolidBlock(BlockID id) {
	return !CanPassThrough(id);
}

bool CanStandOnBlock(int x, int y, int z) {
	BlockID below = IsInWorldBoundaries(x, y - 1, z) ? GetBlock(x, y - 1, z) : BLOCK_AIR;
	BlockID feet = IsInWorldBoundaries(x, y, z) ? GetBlock(x, y, z) : BLOCK_AIR;
	BlockID head = IsInWorldBoundaries(x, y + 1, z) ? GetBlock(x, y + 1, z) : BLOCK_AIR;

	// y == 0 is always solid because it's bedrock, though outside of the world boundaries.
	if (y == 0) {
		return CanPassThrough(feet) && CanPassThrough(head);
	}

	return IsSolidBlock(below) && CanPassThrough(feet) && CanPassThrough(head);
}

BlockID CurrentHoldingBlock() {
	return 0; 
}
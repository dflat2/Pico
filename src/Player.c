#include "ClassiCube/src/Entity.h"

IVec3 Player_GetPosition(void) {
    struct LocalPlayer* player = (struct LocalPlayer*)Entities.List[ENTITIES_SELF_ID];
    struct Entity playerEntity = player->Base;
    Vec3 currentPosition = playerEntity.Position;

    IVec3 position;
    position.x = (int)currentPosition.x;
    position.y = (int)currentPosition.y;
    position.z = (int)currentPosition.z;

    return position;
}

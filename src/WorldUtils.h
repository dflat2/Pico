#ifndef WORLD_UTILS_H
#define WORLD_UTILS_H

#include <stdbool.h>

#include "ClassiCube/src/Core.h"
#include "ClassiCube/src/Vectors.h"

IVec3 SnapToWorldBoundaries(IVec3 position);
IVec3 GetCurrentPlayerPosition();
bool IsInWorldBoundaries(int x, int y, int z);
BlockID GetBlock(int x, int y, int z);

#endif /* WORLD_UTILS_H */

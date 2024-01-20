#ifndef WORLD_UTILS_H
#define WORLD_UTILS_H

#include <stdbool.h>

#include "ClassiCube/src/Core.h"
#include "ClassiCube/src/Vectors.h"

IVec3 SnapToWorldBoundaries(IVec3 position);
IVec3 GetCurrentPlayerPosition();

#endif /* WORLD_UTILS_H */
#ifndef WORLD_UTILS_H
#define WORLD_UTILS_H

#include <stdbool.h>

#include "CC_API/Core.h"
#include "CC_API/Vectors.h"

IVec3 SnapToWorldBoundaries(IVec3 position);
IVec3 GetCurrentPlayerPosition();
bool TryParseCoordinates(const cc_string* coordinates, IVec3* target);
bool IsInWorldBoundaries(int x, int y, int z);
BlockID GetBlock(int x, int y, int z);
bool CanPassThrough(BlockID id);
bool IsSolidBlock(BlockID id);
bool CanStandOnBlock(int x, int y, int z);
BlockID CurrentHoldingBlock();

#endif /* WORLD_UTILS_H */

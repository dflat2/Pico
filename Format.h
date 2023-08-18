#ifndef FORMAT_H
#define FORMAT_H

#include <time.h>

#include "CC_API/Vectors.h"
#include "CC_API/Block.h"

void Format_HHMMSS(time_t time, char* buffer, size_t max);
void Format_Block(BlockID block, char* buffer, size_t max);
void Format_Coordinates(IVec3 coordinates, char* buffer, size_t max);

#endif /* FORMAT_H */

#ifndef FORMAT_H
#define FORMAT_H

#include <time.h>

#include "ClassiCube/src/Vectors.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/PackedCol.h"

void Format_HHMMSS(cc_string* destination, time_t time);
void Format_Block(BlockID block, char* buffer, size_t max);
void Format_Coordinates(IVec3 coordinates, char* buffer, size_t max);
void Format_Int32(cc_string* destination, int integer);
void Format_PackedColor(cc_string* destination, PackedCol color);

#endif

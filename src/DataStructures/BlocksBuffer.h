#ifndef BLOCKS_BUFFER_H
#define BLOCKS_BUFFER_H

#include <stdbool.h>

#include "ClassiCube/src/Vectors.h"

#include "Axis.h"

typedef struct BlocksBuffer_ {
    IVec3 dimensions;
    BlockID* content;
    IVec3 anchor;
} BlocksBuffer;

bool BlocksBuffer_Rotate(Axis axis, int count);
bool BlocksBuffer_Flip(Axis axis);

BlocksBuffer BlocksBuffer_GetCopied(void);
bool BlocksBuffer_IsEmpty(void);
bool BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2, int* out_amountCopied);

#endif

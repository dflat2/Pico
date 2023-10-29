#ifndef BLOCKS_BUFFER_H
#define BLOCKS_BUFFER_H

#include "CC_API/Vectors.h"
#include "CC_API/Core.h"

#include "Axis.h"

typedef struct BlocksBuffer_ {
	IVec3 dimensions;
	BlockID* content;
	IVec3 anchor;
} BlocksBuffer;

bool BlocksBuffer_TryRotate(Axis axis, int count);
bool BlocksBuffer_TryFlip(Axis axis);

BlocksBuffer BlocksBuffer_GetCopied();
bool BlocksBuffer_IsEmpty();
int BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2);

#endif /* BLOCKS_BUFFER_H */

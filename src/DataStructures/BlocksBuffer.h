#ifndef BLOCKS_BUFFER_H
#define BLOCKS_BUFFER_H

#include "ClassiCube/src/Core.h"

#include "VectorsExtension.h"
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
bool BlocksBuffer_TryCopy(IVec3 mark1, IVec3 mark2, int* out_amountCopied);

#endif /* BLOCKS_BUFFER_H */

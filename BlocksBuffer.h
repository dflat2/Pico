#ifndef BLOCKS_BUFFER_H
#define BLOCKS_BUFFER_H

#include "CC_API/Vectors.h"
#include "CC_API/Core.h"

#include "Axis.h"

typedef struct BlocksBuffer_ {
	int width;
	int height;
	int length;
	BlockID* content;
	IVec3 anchor;
} BlocksBuffer;

void BlocksBuffer_Rotate(BlocksBuffer* buffer, Axis axis, int count);
void BlocksBuffer_Flip(BlocksBuffer* buffer, Axis axis);

BlocksBuffer BlocksBuffer_GetCopied();
bool BlocksBuffer_IsEmpty();
int BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2);
void BlocksBuffer_SetCopied(BlocksBuffer buffer);

#endif /* BLOCKS_BUFFER_H */

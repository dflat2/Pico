#ifndef BLOCKS_BUFFER_H
#define BLOCKS_BUFFER_H

#include "CC_API/Vectors.h"
#include "CC_API/Core.h"

typedef struct BlocksBuffer_ {
	int width;
	int height;
	int length;
	BlockID* content;
	IVec3 anchor;
} BlocksBuffer;

BlocksBuffer BlocksBuffer_GetCopied();
bool BlocksBuffer_IsEmpty();
void BlocksBuffer_SetCopied(BlocksBuffer buffer);
int BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2);

#endif /* BLOCKS_BUFFER_H */

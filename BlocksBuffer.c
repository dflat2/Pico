#include "Vectors.h"
#include "BlocksBuffer.h"
#include "MemoryAllocation.h"
#include "WorldUtils.h"

static BlocksBuffer s_buffer = { 0 };
static bool s_bufferIsEmpty = true;

static void FreeBuffer();
static int Pack(int x, int y, int z, int width, int height);

typedef enum S3_ {
	XYZ = 0x00,
	XZY = 0x01,
	ZYX = 0x02,
	YXZ = 0x03,
	ZXY = 0x04,
	YZX = 0x05
} S3;

BlocksBuffer BlocksBuffer_GetCopied() {
	return s_buffer;
}

bool BlocksBuffer_IsEmpty() {
	return s_bufferIsEmpty;
}

int BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2) {
	IVec3 min = Min(mark1, mark2);
	IVec3 max = Max(mark1, mark2);
	IVec3 anchor = Substract(mark1, min);

	int width = max.X - min.X + 1;
	int height = max.Y - min.Y + 1;
	int length = max.Z - min.Z + 1;

	BlockID* blocks = allocateZeros(width * height * length, sizeof(BlockID));
	int index = 0;

	for (int x = min.X; x <= max.X; x++) {
		for (int y = min.Y; y <= max.Y; y++) {
			for (int z = min.Z; z <= max.Z; z++) {
				blocks[index] = GetBlock(x, y, z);
				index++;
			}
		}
	}

	BlocksBuffer buffer = {
		.width = max.X - min.X + 1,
		.height = max.Y - min.Y + 1,
		.length = max.Z - min.Z + 1,
		.content = blocks,
		.anchor = anchor
	};

	BlocksBuffer_SetCopied(buffer);
	return buffer.width * buffer.height * buffer.length;
}

void BlocksBuffer_SetCopied(BlocksBuffer buffer) {
	if (!s_bufferIsEmpty) {
		FreeBuffer();
	}

	s_buffer = buffer;
	s_bufferIsEmpty = false;
}

void BlocksBuffer_Rotate(BlocksBuffer* buffer, Axis axis, int count) {

}

void BlocksBuffer_Flip(BlocksBuffer* buffer, Axis axis) {

}

static void FreeBuffer() {
	if (s_bufferIsEmpty) {
		return;
	}

	free(s_buffer.content);
	s_bufferIsEmpty = true;
}

static void FillTransformedBuffer(BlockID* destination, BlockID* source,
		                          bool flipX, bool flipY, bool flipZ, S3 permutation,
							      int width, int height, int length) {
}

static int Pack(int x, int y, int z, int width, int height) {
	return x + (y * width) + z * (width * height);
}

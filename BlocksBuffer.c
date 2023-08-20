#include "Vectors.h"
#include "BlocksBuffer.h"
#include "MemoryAllocation.h"
#include "WorldUtils.h"

static BlocksBuffer s_buffer = { 0 };
static bool s_bufferIsEmpty = true;

static void FreeBuffer() {
	if (s_bufferIsEmpty) {
		return;
	}

	free(s_buffer.content);
	s_bufferIsEmpty = true;
}

bool BufferIsEmpty() {
	return s_bufferIsEmpty;
}

BlocksBuffer GetCopiedBuffer() {
	return s_buffer;
}

void SetCopiedBuffer(BlocksBuffer buffer) {
	if (!s_bufferIsEmpty) {
		FreeBuffer();
	}

	s_buffer = buffer;
	s_bufferIsEmpty = false;
}

int Copy(IVec3 mark1, IVec3 mark2) {
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

	SetCopiedBuffer(buffer);
	return buffer.width * buffer.height * buffer.length;
}

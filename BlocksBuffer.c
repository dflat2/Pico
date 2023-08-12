#include <stdbool.h>
#include <stdlib.h>

#include "CC_API/Vectors.h"
#include "CC_API/Core.h"

#include "BlocksBuffer.h"

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

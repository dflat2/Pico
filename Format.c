#include <stdio.h>
#include <time.h>

#include "CC_API/Vectors.h"
#include "CC_API/Block.h"
#include "CC_API/String.h"

void Format_HHMMSS(time_t time, char* buffer, size_t size) {
	struct tm* timeStruct = localtime(&time);
	strftime(buffer, sizeof(buffer), "%X", timeStruct);
}

void Format_Block(BlockID block, char* buffer, size_t size) {
	cc_string cc_blockName = Block_UNSAFE_GetName(block);

	for (int i = 0; i < cc_blockName.length; i++) {
		if (i == size - 1) {
			buffer[i] = '\0';
			return;
		}

		buffer[i] = cc_blockName.buffer[i];
	}

	buffer[cc_blockName.length] = '\0';
}

void Format_Coordinates(IVec3 coordinates, char* buffer, size_t size) {
	snprintf(buffer, size, "(%d, %d, %d)", coordinates.X, coordinates.Y, coordinates.Z);
}

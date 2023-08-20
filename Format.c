#include <stdio.h>
#include <time.h>

#include "CC_API/Block.h"

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

void Format_Int32(int integer, char* buffer, size_t max) {
	if (integer == 0 && max >= 2) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	int32_t integer32 = (int32_t)integer;

	// "-2,147,483,648" (-2^31) has 14 characters. Add one for '\0'.
	char reversedResult[15] = { 0 };
	bool isNegative = (integer < 0);

	if (isNegative) {
		integer32 = -integer32;
	}

	int integerTruncated = integer32;
	int commaCounter = 0;

	reversedResult[0] = '\0';
	int i = 1;

	while (integerTruncated != 0) {
		if (commaCounter == 3) {
			reversedResult[i] = ',';
			i++;
			commaCounter = 0;
		}

		reversedResult[i] = (integerTruncated % 10) + '0';
		i++;
		commaCounter++;
		integerTruncated /= 10;
	}

	if (isNegative) {
		reversedResult[i] = '-';
		i++;
	}

	i--;
	int j = 0;

	while (i >= 0 && j < max) {
		buffer[j] = reversedResult[i];
		i--;
		j++;
	}
}

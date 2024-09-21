#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ClassiCube/src/PackedCol.h"
#include "ClassiCube/src/String.h"

#include "Format.h"

void Format_HHMMSS(cc_string* destination, time_t time) {
    struct tm* timeStruct = localtime(&time);
    destination->length = strftime(destination->buffer, destination->capacity, "%X", timeStruct);
}

void Format_Block(BlockID block, char* buffer, size_t size) {
    cc_string cc_blockName = Block_UNSAFE_GetName(block);

    for (int i = 0; i < cc_blockName.length; i++) {
        if ((size_t)i == size - 1) {
            buffer[i] = '\0';
            return;
        }

        buffer[i] = cc_blockName.buffer[i];
    }

    buffer[cc_blockName.length] = '\0';
}

void Format_Coordinates(IVec3 coordinates, char* buffer, size_t size) {
    snprintf(buffer, size, "(%d, %d, %d)", coordinates.x, coordinates.y, coordinates.z);
}

void Format_Int32(cc_string* destination, int integer) {
    if (integer == 0 && destination->capacity >= 2) {
        destination->buffer[0] = '0';
        destination->length = 1;
        return;
    }

    int32_t integer32 = (int32_t)integer;

    // Longest possible result: "-2,147,483,648" (-2^31) has 14 characters.
    char reversedResult[14] = { 0 };
    bool isNegative = (integer < 0);

    if (isNegative) {
        integer32 = -integer32;
    }

    int integerTruncated = integer32;
    int commaCounter = 0;

    int i = 0;

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

    while (i >= 0 && j < destination->capacity) {
        destination->buffer[j] = reversedResult[i];
        i--;
        j++;
    }

    destination->length = j;
}

void Format_PackedColor(cc_string* destination, PackedCol color) {
    char hexadecimalDigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned char red = (unsigned char)PackedCol_R(color);
    unsigned char green = (unsigned char)PackedCol_G(color);
    unsigned char blue = (unsigned char)PackedCol_B(color);

    char colorString[8];

    colorString[0] = '#';
    colorString[1] = hexadecimalDigits[red >> 4];
    colorString[2] = hexadecimalDigits[red & 0x0F];
    colorString[3] = hexadecimalDigits[green >> 4];
    colorString[4] = hexadecimalDigits[green & 0x0F];
    colorString[5] = hexadecimalDigits[blue >> 4];
    colorString[6] = hexadecimalDigits[blue & 0x0F];
    colorString[7] = '\0';

    String_Format1(destination, "%c", colorString);
}

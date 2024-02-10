#include "ClassiCube/src/Block.h"

#include "Brushes/Brush.h"
#include "Message.h"
#include "Parse.h"
#include "Player.h"

typedef enum TimeUnit_ {
    UNIT_SECOND = 0x01,
    UNIT_MINUTE = 0x02,
    UNIT_HOUR = 0x04,
    UNIT_DAY = 0x08,
    UNIT_NULL = 0x80,
} TimeUnit;

static bool TryParseDuration_Second(const cc_string* string, int* cursor, int* out_result_Second, TimeUnit* out_setUnit);
static bool TryParseSingleCoordinate(const cc_string* coordinateString, int* out_result, bool* out_isRelative);
static bool TryParseTimeUnit(const cc_string* string, int* cursor, TimeUnit* out_unit);
static bool TryParsePositiveNumber(const cc_string* string, int* cursor, int* out_number);
static bool IsDigit(char character);

bool Parse_TryParseBlock(const cc_string* blockString, BlockID* block) {
    int i_block;
    i_block = Block_Parse(blockString);

    if (i_block == -1) {
        Message_ShowUnknownBlock(blockString);
        return false;
    }

    *block = (BlockID)i_block;
    return true;
}

bool Parse_LastArgumentIsRepeat(const cc_string* arguments, int* ref_count) {
    if (*ref_count == 0) {
        return false;
    }

    const cc_string* lastArgument = &arguments[*ref_count - 1];

    if (lastArgument->length == 1 && lastArgument->buffer[0] == '+') {
        *ref_count = *ref_count - 1;
        return true;
    }
    
    return false;
}

bool Parse_TryParseDeltaTime_Second(const cc_string* string, int* out_total_Second) {
    *out_total_Second = 0;

    if (string == NULL || string->length == 0) {
        return false;
    }

    int index = 0;
    int* cursor = &index;
    int out_duration_Second;
    TimeUnit out_unit;
    TimeUnit highestSetUnit = UNIT_NULL;

    while (index < string->length) {
        if (!TryParseDuration_Second(string, cursor, &out_duration_Second, &out_unit)) {
            return false;
        }

        // Wrong units order, for example 2sec5min is invalid, but 5min2sec is valid.
        if (highestSetUnit <= out_unit) {
            return false;
        }

        highestSetUnit = out_unit;
        *out_total_Second += out_duration_Second;
    }

    return true;
}

void Parse_ShowExamplesDeltaTime(void) {
    Message_Player("Duration must be of the form &b[count]h[count]m[count]s&f.");
    Message_Player("Example: &b1h30m&f means one hour and thirty minutes.");
}

bool Parse_TryParseBrush(const cc_string* arguments, int argumentsCount) {
    bool startsWithAtSign = String_IndexOfAt(&arguments[0], 0, '@') == 0;
    if (!startsWithAtSign) {
        return false;
    }

    return Brush_TryLoad(&arguments[0], &arguments[1], argumentsCount - 1);
}

bool Parse_TryParseBlockOrBrush(const cc_string* arguments, int argumentsCount) {
    bool startsWithAtSign = String_IndexOfAt(&arguments[0], 0, '@') == 0;

    if (startsWithAtSign) {
        return Parse_TryParseBrush(arguments, argumentsCount);
    } else if (argumentsCount > 1) {
        Message_Player("Trailing characters in command.");
        return false;
    }

    if (!Brush_TryLoadSolid(&arguments[0])) {
        return false;
    }

    return true;
}

bool Parse_TryParseAxis(const cc_string* string, Axis* out_axis) {
    if (string->length == 0 || string->length >= 2) {
        Message_ShowInvalidAxis(string);
        return false;
    }

    char firstCharacter = string->buffer[0];

    if (firstCharacter == 'X' || firstCharacter == 'x') {
        *out_axis = AXIS_X;
        return true;
    } else if (firstCharacter == 'Y' || firstCharacter == 'y') {
        *out_axis = AXIS_Y;
        return true;
    } else if (firstCharacter == 'Z' || firstCharacter == 'z') {
        *out_axis = AXIS_Z;
        return true;
    }

    Message_ShowInvalidAxis(string);
    return false;
}

bool Parse_TryParseDegrees(const cc_string* string, int* out_degrees) {
    if (!Parse_TryParseNumber(string, out_degrees)) {
        Message_ShowInvalidDegrees(string);
        return false;
    }

    *out_degrees = (*out_degrees) % 360;

    if (*out_degrees % 90 != 0) {
        Message_ShowInvalidDegrees(string);
        return false;
    }

    return true;
}

static bool IsDigit(char character) {
    return (character >= '0' && character <= '9');
}

static bool TryParsePositiveNumber(const cc_string* string, int* cursor, int* out_number) {
    if (*cursor >= string->length || !IsDigit(string->buffer[*cursor])) {
        return false;
    }

    *out_number = 0;

    while (IsDigit(string->buffer[*cursor])) {
        *out_number = *out_number * 10 + (string->buffer[*cursor] - '0');
        (*cursor)++;

        if (*cursor >= string->length) {
            break;
        }
    }

    return true;
}

bool Parse_TryParseNumber(const cc_string* string, int* out_number) {
    bool isNegative = false;
    int start = 0;

    if (string->buffer[0] == '-') {
        ++start;
        isNegative = true;
    }

    if (!TryParsePositiveNumber(string, &start, out_number)) {
        char invalidNumberMessageBuffer[64];
        cc_string invalidNumberMessage = String_FromArray(invalidNumberMessageBuffer);
        String_Format1(&invalidNumberMessage, "Invalid integer: &b%s&f.", string);
        Chat_Add(&invalidNumberMessage);
        return false;
    }

    if (isNegative) {
        *out_number = -(*out_number);
    }

    return true;
}

bool Parse_TryParseFloat(const cc_string* string, float* out_float) {
    bool success = Convert_ParseFloat(string, out_float);

    if (!success) {
        char invalidFloatMessageBuffer[STRING_SIZE];
        cc_string invalidFloatMessage = String_FromArray(invalidFloatMessageBuffer);
        String_Format1(&invalidFloatMessage, "Invalid decimal: &b%s&f.", string);
        Chat_Add(&invalidFloatMessage);
        return false;
    }

    return true;
}

static bool TryParseTimeUnit(const cc_string* string, int* cursor, TimeUnit* out_unit) {
    if (*cursor >= string->length) {
        return false;
    }

    if (string->buffer[*cursor] == 'd') {
        (*cursor)++;
        *out_unit = UNIT_DAY;
        return true;
    } if (string->buffer[*cursor] == 'h') {
        (*cursor)++;
        *out_unit = UNIT_HOUR;
        return true;
    } else if (string->buffer[*cursor] == 'm') {
        (*cursor)++;

        // If it's "min" instead of just "m", move forward by 2 more steps.
        if (*cursor <= (string->length - 2) && string->buffer[(*cursor)] == 'i' && string->buffer[(*cursor) + 1] == 'n') {
            *cursor += 2;
        }

        *out_unit = UNIT_MINUTE;
        return true;
    } else if (string->buffer[*cursor] == 's') {
        (*cursor)++;

        // If it's "sec" instead of just "s", move forward by 2 more steps.
        if (*cursor <= (string->length - 2) && string->buffer[(*cursor)] == 'e'  && string->buffer[(*cursor) + 1] == 'c') {
            *cursor += 2;
        }

        *out_unit = UNIT_SECOND;
        return true;
    }

    return false;
}

static bool TryParseDuration_Second(const cc_string* string, int* cursor, int* out_result_Second, TimeUnit* out_setUnit) {
    if (!TryParsePositiveNumber(string, cursor, out_result_Second)) {
        return false;
    }

    if (!TryParseTimeUnit(string, cursor, out_setUnit)) {
        return false;
    }

    switch (*out_setUnit) {
        case UNIT_SECOND: 
            break;
        case UNIT_MINUTE: 
            *out_result_Second *= 60;
            break;
        case UNIT_HOUR: 
            *out_result_Second *= 3600;
            break;
        case UNIT_DAY: 
            *out_result_Second *= 86400;
            break;
        default:
            return false;
    }

    return true;
}

static bool TryParseSingleCoordinate(const cc_string* coordinateString, int* out_result, bool* out_isRelative) {
    char first = coordinateString->buffer[0];
    char last = coordinateString->buffer[coordinateString->length - 1];
    *out_isRelative = (first == '(') && (last == ')');

    cc_string number = { coordinateString->buffer, coordinateString->length, coordinateString->capacity };

    if (*out_isRelative) {
        number.buffer = &(number.buffer[1]);
        number.length -= 2;
    }

    if (Convert_ParseInt(&number, out_result)) {
        return true;
    }

    return false;
}

static void CoordinateError(const cc_string* coordinate) {
    char error[64];
    cc_string cc_error = { error, 0, 64 };
    String_Format1(&cc_error, "Could not parse coordinate &b%s&f.", coordinate);
    Chat_Add(&cc_error);
}

bool Parse_TryParseCoordinates(const cc_string* coordinates, IVec3* out_result) {
    IVec3 playerPosition = Player_GetPosition();

    int arrayTarget[3];
    int arrayPlayerPosition[3] = { playerPosition.X, playerPosition.Y, playerPosition.Z };
    bool isRelative;

    for (int i = 0; i < 3; i++) {
        if (!TryParseSingleCoordinate(&coordinates[i], &arrayTarget[i], &isRelative)) {
            CoordinateError(&coordinates[i]);
            return false;
        }

        if (isRelative) {
            arrayTarget[i] += arrayPlayerPosition[i];
        }
    }

    out_result->X = arrayTarget[0];
    out_result->Y = arrayTarget[1];
    out_result->Z = arrayTarget[2];
    return true;
}

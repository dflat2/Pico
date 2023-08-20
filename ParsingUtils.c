#include "CC_API/String.h"
#include "CC_API/Block.h"

#include "Messaging.h"
#include "ParsingUtils.h"

#define SUCCESS 1
#define FAILURE 0

typedef enum TimeUnit_ {
	UNIT_SECOND = 0x01,
	UNIT_MINUTE = 0x02,
	UNIT_HOUR = 0x04,
	UNIT_DAY = 0x08,
	UNIT_NULL = 0x80,
} TimeUnit;

static bool TryParseDuration_Second(const cc_string* string, int* cursor, int* out_result_Second, TimeUnit* out_setUnit);
static bool TryParseDuration_Second(const cc_string* string, int* cursor, int* out_result_Second, TimeUnit* out_setUnit);
static bool TryParseTimeUnit(const cc_string* string, int* cursor, TimeUnit* out_unit);
static bool TryParsePositiveNumber(const cc_string* string, int* cursor, int* out_number);
static bool TryParsePositiveNumber(const cc_string* string, int* cursor, int* out_number);
static bool IsDigit(char character);

bool TryParseBlock(const cc_string* blockString, BlockID* block) {
    int i_block;
    i_block = Block_Parse(blockString);

    if (i_block == -1) {
		Message_ShowUnknownBlock(blockString);
        return FAILURE;
    }

    *block = (BlockID)i_block;
    return SUCCESS;
}

bool Parse_CommandFunc(cc_string argument, NamedCommandFunc* commands, int count, CommandFunc* out_function) {
    for (int i = 0; i < count; i++) {
		if (String_CaselessEquals(&argument, &(commands[i].name))) {
			*out_function = commands[i].function;
			return SUCCESS;
		}
	}

	*out_function = NULL;
	return FAILURE;
}

bool Parse_DeltaTime_Second(const cc_string* string, int* out_total_Second) {
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

void Parse_ShowExamplesDeltaTime() {
	PlayerMessage("Duration must be of the form &b[count]h[count]m[count]s&f.");
	PlayerMessage("Example: &b1h30m&f means one hour and thirty minutes.");
}

bool Parse_TryParseBrush(const cc_string* arguments, int argumentsCount, Brush* out_brush) {
	bool startsWithAtSign = String_IndexOfAt(&arguments[0], 0, '@') == 0;
	if (!startsWithAtSign) {
		return FAILURE;
	}

	return Brush_TryCreate(&arguments[0], &arguments[1], argumentsCount - 1, out_brush);
}

bool Parse_TryParseBlockOrBrush(const cc_string* arguments, int argumentsCount, Brush* out_brush) {
	bool startsWithAtSign = String_IndexOfAt(&arguments[0], 0, '@') == 0;

	if (!startsWithAtSign) {
		BlockID block = 0;

		if (!TryParseBlock(&arguments[0], &block)) {
			return false;
		}

		Brush_TryCreateSolid(block, out_brush);
		return true;
	}

	return Parse_TryParseBrush(arguments, argumentsCount, out_brush);
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

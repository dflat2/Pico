#include <string.h>
#include <stdio.h>

#include "CC_API/String.h"
#include "CC_API/Inventory.h"
#include "CC_API/Chat.h"
#include "CC_API/Block.h"

#include "Messaging.h"
#include "ParsingUtils.h"

#define SUCCESS 1
#define FAILURE 0

static void ShowUnknownMode(const cc_string* mode) {
    char message[64];
    cc_string cc_message = { message, .length = 0, .capacity = sizeof(message) };
    String_Format1(&cc_message, "&fUnkown mode: &b%s&f.", mode);
    Chat_Add(&cc_message);
}

static void ShowAvailableModes(const cc_string* modes, int modesCount) {
    char buffer[128];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_AppendConst(&message, "&fAvailable modes: &b");

    for (int i = 0; i < modesCount - 1; i++) {
        String_AppendString(&message, &modes[i]);
        String_AppendConst(&message, "&f, &b");
    }

    String_AppendString(&message, &modes[modesCount - 1]);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
}

static void ShowUnkownBlock(const cc_string* block) {
    char buffer[128];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_Format1(&message, "&fCould not find block specified: &b%s&f.", block);
    Chat_Add(&message);
}

bool TryParseBlock(const cc_string* blockString, BlockID* block) {
    int i_block;
    i_block = Block_Parse(blockString);

    if (i_block == -1) {
        ShowUnkownBlock(blockString);
        return FAILURE;
    }

    *block = (BlockID)i_block;
    return SUCCESS;
}

bool TryParseMode(const cc_string* modes, int modesCount, const cc_string* modeString, int* mode) {
    for (int i = 0; i < modesCount; i++) {
       if (String_CaselessEquals(&modes[i], modeString)) {
            *mode = i;
            return SUCCESS;
       }
    }

    ShowUnknownMode(modeString);
    ShowAvailableModes(modes, modesCount);
    return FAILURE;
}

static bool IsMode(const cc_string* argument) {
    cc_string modePrefix = String_FromConst(":");
    return String_CaselessStarts(argument, &modePrefix);
}

bool TryParseModeBlock(const cc_string* modes, const int modesCount, const cc_string* args, const int argsCount,
                       int* mode, BlockID* block) {
    *mode = 0;
    *block = Inventory_SelectedBlock;

    bool noArguments = argsCount == 0;
    bool justMode = (argsCount == 1) && IsMode(&args[0]);
    bool justBlock = (argsCount == 1) && !IsMode(&args[0]);
    bool modeAndBlock = (argsCount == 2);

    if (noArguments) {
        return SUCCESS;
    }
    else if (justMode) {
        return TryParseMode(modes, modesCount, &args[0], mode);
    }
    else if (justBlock) {
        return TryParseBlock(&args[0], block);
    }
    else if (modeAndBlock) {
        return (TryParseMode(modes, modesCount, &args[0], mode) && TryParseBlock(&args[1], block));
    }

    PlayerMessage("&fThis command only takes two arguments.");
    return FAILURE;
}

int Parse_IndexOfStringCaseless(cc_string argument, cc_string* strings, int count) {
    for (int i = 0; i < count; i++) {
		if (String_CaselessEquals(&argument, &strings[i])) {
			return i;
		}
	}

    return -1;
}

bool Parse_CommandFunc(cc_string argument, NamedCommandFunc* commands, int count, CommandFunc* function) {
    for (int i = 0; i < count; i++) {
		if (String_CaselessEquals(&argument, &(commands[i].name))) {
			*function = commands[i].function;
			return true;
		}
	}

	return false;
}

typedef enum TimeUnit_ {
	UNIT_SECOND = 0x01,
	UNIT_MINUTE = 0x02,
	UNIT_HOUR = 0x04,
	UNIT_DAY = 0x08,
	UNIT_NULL = 0x80,
} TimeUnit;

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

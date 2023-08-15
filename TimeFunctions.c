#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "TimeFunctions.h"

long Time_Now_Millisecond() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long milliseconds = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return milliseconds;
}

int Time_FormatDayTime(char* out_formattedTime, size_t size, DayTime dayTime) {
    return snprintf(out_formattedTime, size, "%02d:%02d", dayTime.hours, dayTime.minutes);
}

DayTime Time_UnixTimeToDayTime(double unixTime_Second) {
    DayTime dayTime;

    // Convert the Unix time to seconds, then extract hours, minutes, and seconds
    int totalSeconds = (int)unixTime_Second;
    dayTime.seconds = totalSeconds % 60;
    int totalMinutes = totalSeconds / 60;
    dayTime.minutes = totalMinutes % 60;
    int totalHours = totalMinutes / 60;
    dayTime.hours = totalHours % 24;

    return dayTime;
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

static bool IsNonZeroDigit(char character) {
    return (character >= '1' && character <= '9');
}

static bool TryParsePositiveNumber(char* string, int* out_number, int* index) {
    *out_number = 0;

    while (IsDigit(string[*index])) {
        *out_number = *out_number * 10 + (string[*index] - '0');
        (*index) = (*index) + 1;
    }

    return true;
}

static bool TryParseTimeUnit(char* string, int* index, TimeUnit* out_unit) {
	if (string[*index] == '\0') {
		return false;
	}

	if (string[*index] == 'd') {
		(*index)++;
		*out_unit = UNIT_DAY;
		return true;
	} if (string[*index] == 'h') {
		(*index)++;
		*out_unit = UNIT_HOUR;
		return true;
	} else if (string[*index] == 'm') {
		(*index)++;

		// If it's "min" instead of just "m", move forward by 2 more steps.
		if (string[(*index) + 1] == 'i'  && string[(*index) + 2] == 'n') {
			*index += 2;
		}

		*out_unit = UNIT_MINUTE;
		return true;
	} else if (string[*index] == 's') {
		(*index)++;

		// If it's "sec" instead of just "s", move forward by 2 more steps.
		if (string[(*index) + 1] == 'e'  && string[(*index) + 2] == 'c') {
			*index += 2;
		}

		*out_unit = UNIT_SECOND;
		return true;
	}

	return false;
}

static bool TryParseDuration_Second(char* string, int* index, int* out_result_Second, TimeUnit* out_setUnit) {
	if (!TryParsePositiveNumber(string, index, out_result_Second)) {
		return false;
	}

	if (!TryParseTimeUnit(string, index, out_setUnit)) {
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

bool Time_TryParseDeltaTime_Second(char* string, int* out_result_Second) {
    if (string == NULL || strlen(string) == 0) {
        return false;
    }

	int index = 0;
	*out_result_Second = 0;
	TimeUnit setUnits = UNIT_NULL;

	int lastParsed_Second;
	TimeUnit currentUnit;
	bool wrongUnitsOrder;

	while (string[index] != '\0') {
		if (!TryParseDuration_Second(string, &index, &lastParsed_Second, &currentUnit)) {
			return false;
		}

		wrongUnitsOrder = (currentUnit > setUnits);

		if (wrongUnitsOrder) {
			return false;
		}

		setUnits |= currentUnit;
		*out_result_Second += lastParsed_Second;
	}

	return true;
}

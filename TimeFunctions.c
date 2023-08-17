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

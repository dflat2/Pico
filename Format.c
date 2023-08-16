#include <stdio.h>

#include "TimeFunctions.h"

void Format_HHMMSS(UnixTimeStamp_S time, char* buffer, size_t max) {
    int totalSeconds = (int)time;
    int seconds = totalSeconds % 60;
    int totalMinutes = totalSeconds / 60;
    int minutes = totalMinutes % 60;
    int totalHours = totalMinutes / 60;
    int hours = totalHours % 24;
    snprintf(buffer, max, "%02d:%02d:%02d", hours, minutes, seconds); 
}

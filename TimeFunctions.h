#ifndef TIME_H
#define TIME_H

typedef struct DayTime_ {
    int hours; // 0 - 23
    int minutes; // 0 - 59
    int seconds; // 0 - 59
} DayTime;

long Time_Now_Millisecond();
int Time_FormatDayTime(char* out_formattedTime, size_t size, DayTime dayTime);
DayTime Time_UnixTimeToDayTime(double unixTime);
bool Time_TryParseDeltaTime_Second(char* string, int* out_result_Second);

#endif

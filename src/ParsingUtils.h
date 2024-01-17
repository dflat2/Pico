#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "ClassiCube/src/String.h"

#include "DataStructures/Axis.h"
#include "Brushes/Brush.h"

bool Parse_TryParseBlock(const cc_string* blockString, BlockID* block);
bool Parse_LastArgumentIsRepeat(const cc_string* arguments, int count);
bool Parse_TryParseDeltaTime_Second(const cc_string* string, int* out_result_Second);
void Parse_ShowExamplesDeltaTime();
bool Parse_TryParseNumber(const cc_string* string, int* out_number);
bool Parse_TryParseBlockOrBrush(const cc_string* arguments, int argumentsCount);
bool Parse_TryParseAxis(const cc_string* string, Axis* out_axis);
bool Parse_TryParseDegrees(const cc_string* string, int* out_degrees);

#endif /* PARSING_UTILS_H */

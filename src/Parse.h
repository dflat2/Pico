#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include <stdbool.h>

#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Vectors.h"
#include "ClassiCube/src/PackedCol.h"

#include "DataStructures/Axis.h"

bool Parse_TryParseBlock(const cc_string* blockString, BlockID* block);
bool Parse_LastArgumentIsRepeat(const cc_string* arguments, int* ref_count);
bool Parse_TryParseDeltaTime_Second(const cc_string* string, int* out_result_Second);
void Parse_ShowExamplesDeltaTime(void);
bool Parse_TryParseNumber(const cc_string* string, int* out_number);
bool Parse_TryParseBlockOrBrush(const cc_string* arguments, int argumentsCount);
bool Parse_TryParseAxis(const cc_string* string, Axis* out_axis);
bool Parse_TryParseDegrees(const cc_string* string, int* out_degrees);
bool Parse_TryParseCoordinates(const cc_string* coordinates, IVec3* out_result);
bool Parse_TryParseFloat(const cc_string* string, float* out_float);
bool Parse_TryParseColor(const cc_string* colorString, PackedCol* out_result);

#endif

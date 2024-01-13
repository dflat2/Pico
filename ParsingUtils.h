#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "ClassiCube/src/String.h"

#include "Axis.h"
#include "Brush.h"

typedef void (*CommandFunc)(const cc_string* args, int argsCount);

typedef struct NamedCommandFunc_ {
	cc_string name;
	CommandFunc function;
} NamedCommandFunc;

bool TryParseBlock(const cc_string* blockString, BlockID* block);
bool Parse_CommandFunc(cc_string argument, NamedCommandFunc* commands, int count, CommandFunc* out_function);
bool Parse_DeltaTime_Second(const cc_string* string, int* out_result_Second);
void Parse_ShowExamplesDeltaTime();
bool Parse_TryParseNumber(const cc_string* string, int* out_number);
bool Parse_TryParseBlockOrBrush(const cc_string* arguments, int argumentsCount);
bool Parse_TryParseAxis(const cc_string* string, Axis* out_axis);
bool Parse_TryParseDegrees(const cc_string* string, int* out_degrees);

#endif /* PARSING_UTILS_H */

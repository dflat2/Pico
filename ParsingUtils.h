#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "CC_API/String.h"

typedef void (*CommandFunc)(const cc_string* args, int argsCount);

typedef struct NamedCommandFunc_ {
	cc_string name;
	CommandFunc function;
} NamedCommandFunc;

bool TryParseBlock(const cc_string* blockString, BlockID* block);
bool TryParseModeBlock(const cc_string* modes, const int modesCount, const cc_string* args, const int argsCount,
                       int* mode, BlockID* block);
bool TryParseMode(const cc_string* modes, int modesCount, const cc_string* modeString, int* mode);
int Parse_IndexOfStringCaseless(cc_string argument, cc_string* strings, int count);
bool Parse_CommandFunc(cc_string argument, NamedCommandFunc* commands, int count, CommandFunc* out_function);
bool Parse_DeltaTime_Second(char* string, int* out_result_Second);

#endif /* PARSING_UTILS_H */

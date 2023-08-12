#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "CC_API/String.h"

bool TryParseBlock(const cc_string* blockString, BlockID* block);
bool TryParseModeBlock(const cc_string* modes, const int modesCount, const cc_string* args, const int argsCount,
                       int* mode, BlockID* block);
bool TryParseMode(const cc_string* modes, int modesCount, const cc_string* modeString, int* mode);

#endif /* PARSING_UTILS_H */

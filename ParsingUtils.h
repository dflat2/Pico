#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "CC_API/String.h"

bool TryParseModeBlock(const cc_string* modes, const int modesCount, const cc_string* args, const int argsCount,
                       int* mode, BlockID* block);

#endif /* PARSING_UTILS_H */

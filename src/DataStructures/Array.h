#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>

#include "ClassiCube/src/Core.h"

int Array_IndexOfStringCaseless(const cc_string* element, const cc_string* list, int listCount);
bool Array_ContainsString(const cc_string* element, const cc_string* list, int listCount);

#endif

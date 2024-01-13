#ifndef ARRAY_H
#define ARRAY_H

#include <stdbool.h>

#include "ClassiCube/src/String.h"

#define ARRAY_CONTAINS_STRING(str, array) \ Array_ContainsString(str, array, sizeof(array) / sizeof(array[0]))

int Array_IndexOfStringCaseless(const cc_string* element, const cc_string* list, int listCount);
bool Array_ContainsString(const cc_string* element, const cc_string* list, int listCount);

#endif /* ARRAY_H */

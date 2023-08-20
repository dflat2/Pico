#include "CC_API/String.h"

#include "Array.h"

int Array_IndexOfStringCaseless(const cc_string* element, const cc_string* list, int listCount) {
    for (int i = 0; i < listCount; i++) {
		if (String_CaselessEquals(element, &list[i])) {
			return i;
		}
	}

    return -1;
}

bool Array_ContainsString(const cc_string* element, const cc_string* list, int listCount) {
	return Array_IndexOfStringCaseless(element, list, listCount) != -1;
}

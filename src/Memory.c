#include <stdlib.h>
#include <stdbool.h>

static bool s_Error;

void* Memory_Allocate(size_t size) {
    s_Error = false;
    void* allocated = malloc(size);

    if (allocated == NULL) {
        s_Error = true;
        return NULL;
    }

    return allocated;
}

void* Memory_Reallocate(void* pointer, size_t size) {
    s_Error = false;
    void* allocated = realloc(pointer, size);

    if (allocated == NULL) {
        s_Error = true;
        return NULL;
    }

    return allocated;
}

void* Memory_AllocateZeros(int count, size_t size) {
    s_Error = false;
    void* allocated = calloc(count, size);

    if (allocated == NULL) {
        s_Error = true;
        return NULL;
    }

    return allocated;
}

bool Memory_AllocationError(void) {
    return s_Error;
}

void Memory_HandleError(void) {
    s_Error = false;
}

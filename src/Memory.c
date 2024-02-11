#include <stdlib.h>
#include <stdbool.h>

void* Memory_Allocate(size_t size) {
    void* allocated = malloc(size);

    if (allocated == NULL && size != 0) {
        abort();
    }

    return allocated;
}

void* Memory_Reallocate(void* pointer, size_t size) {
    void* allocated = realloc(pointer, size);

    if (allocated == NULL && size != 0) {
        abort();
    }

    return allocated;
}

void* Memory_AllocateZeros(int count, size_t size) {
    void* allocated = calloc(count, size);

    if (allocated == NULL && size != 0) {
        abort();
    }

    return allocated;
}

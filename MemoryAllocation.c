#include <stdlib.h>

void* allocate(unsigned count, size_t size) {
    void* result = malloc(count * size);

    if (result == NULL) {
        exit(1);
    }

    return result;
}

void* allocateZeros(unsigned count, size_t size) {
    void* result = calloc(count, size);

    if (result == NULL) {
        exit(1);
    }

    return result;
}
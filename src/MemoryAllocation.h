#ifndef MEMORY_ALLOCATION_H
#define MEMORY_ALLOCATION_H

#include <stdlib.h>

void* allocate(unsigned count, size_t size);
void* allocateZeros(unsigned count, size_t size);

#endif /* MEMORY_ALLOCATION_H */

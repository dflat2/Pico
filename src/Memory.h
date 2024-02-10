#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>

void* Memory_Allocate(size_t size);
void* Memory_Reallocate(void* pointer, size_t size);
void* Memory_AllocateZeros(int count, size_t size);
bool Memory_AllocationError(void);
void Memory_HandleError(void);

#endif

#include "ClassiCube/src/Vectors.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct IVec3FastQueue_ {
    IVec3* allocated;
    int totalAllocated;
    int cursorFirstElement;
    int length;
}IVec3FastQueue;

static bool TryAllocateMore(IVec3FastQueue* queue);
static bool IsSaturated(IVec3FastQueue* queue);

IVec3FastQueue* IVec3FastQueue_CreateEmpty(void) {
    IVec3FastQueue* queue = (IVec3FastQueue*)calloc(1, sizeof(IVec3FastQueue));
    return queue;
}

IVec3 IVec3FastQueue_Dequeue(IVec3FastQueue* queue) {
    IVec3 dequeued = queue->allocated[queue->cursorFirstElement];
    queue->cursorFirstElement++;
    queue->length--;
    return dequeued;
}

bool IVec3FastQueue_IsEmpty(IVec3FastQueue* queue) {
    return queue->length == 0;
}

bool IVec3FastQueue_TryEnqueue(IVec3FastQueue* queue, IVec3 vector) {
    if (IsSaturated(queue) && !TryAllocateMore(queue)) {
        return false;
    }

    queue->allocated[queue->cursorFirstElement + queue->length] = vector;
    queue->length++;
    return true;
}

void IVec3FastQueue_Free(IVec3FastQueue* queue) {
    free(queue->allocated);
    free(queue);
}

static bool TryAllocateMore(IVec3FastQueue* queue) {
    int newTotalAllocated = (queue->totalAllocated + 1) * 2;

    IVec3* newAllocated = realloc(queue->allocated, newTotalAllocated * sizeof(IVec3));

    if (newAllocated == NULL) {
        return false;
    }

    queue->totalAllocated = newTotalAllocated;
    queue->allocated = newAllocated;

    return true;
}

static bool IsSaturated(IVec3FastQueue* queue) {
    return queue->cursorFirstElement + queue->length >= queue->totalAllocated;
}

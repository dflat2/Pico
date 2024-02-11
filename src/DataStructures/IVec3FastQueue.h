#ifndef I_VEC3_FAST_QUEUE_H
#define I_VEC3_FAST_QUEUE_H

#include "ClassiCube/src/Vectors.h"

#include <stdbool.h>

typedef struct IVec3FastQueue_ IVec3FastQueue;

IVec3FastQueue* IVec3FastQueue_CreateEmpty(void);
IVec3 IVec3FastQueue_Dequeue(IVec3FastQueue* queue);
bool IVec3FastQueue_IsEmpty(IVec3FastQueue* queue);
void IVec3FastQueue_Enqueue(IVec3FastQueue* queue, IVec3 vector);
void IVec3FastQueue_Free(IVec3FastQueue* queue);

#endif

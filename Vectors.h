#ifndef VECTORS_H
#define VECTORS_H

#include "CC_API/Vectors.h"
#include "Axis.h"

typedef struct IVec2_ {
    int X;
    int Y;
} IVec2;

IVec3 Min(IVec3 a, IVec3 b);
IVec3 Max(IVec3 a, IVec3 b);
IVec3 Add(const IVec3 a, const IVec3 b);
IVec3 Transform2DTo3D(IVec2 vector, Axis axis);
double Distance(IVec3 a, IVec3 b);
IVec3 Substract(const IVec3 a, const IVec3 b);
int Dot(const IVec3 a, const IVec3 b);

#endif /* VECTORS_H */

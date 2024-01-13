#ifndef VECTORS_H
#define VECTORS_H

#include "ClassiCube/src/Vectors.h"

#include "Axis.h"

typedef struct IVec2_ {
    int X;
    int Y;
} IVec2;

typedef struct FVec3_ {
	float X;
	float Y;
	float Z;
} FVec3;

IVec3 Min(IVec3 a, IVec3 b);
IVec3 Max(IVec3 a, IVec3 b);
IVec3 Add(const IVec3 a, const IVec3 b);
IVec3 Transform2DTo3D(IVec2 vector, Axis axis);
double Distance(IVec3 a, IVec3 b);
IVec3 Substract(const IVec3 a, const IVec3 b);
int Dot(const IVec3 a, const IVec3 b);
FVec3 FVec3_ScalarMultiply(const FVec3 vector, float t);
FVec3 FVec3_Add(const FVec3 a, const FVec3 b);
FVec3 FVec3_Substract(const FVec3 a, const FVec3 b);
FVec3 IVec3_ConvertFVec3(const IVec3 a);
IVec3 FVec3_ConvertIVec3(const FVec3 a);

#endif /* VECTORS_H */

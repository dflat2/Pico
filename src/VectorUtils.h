#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include "ClassiCube/src/Vectors.h"

#include "DataStructures/Axis.h"

typedef struct IVec2_ {
    int x;
    int y;
} IVec2;

typedef struct FVec3_ {
    float x;
    float y;
    float z;
} FVec3;

IVec3 VectorUtils_IVec3_Min(IVec3 a, IVec3 b);
IVec3 VectorUtils_IVec3_Max(IVec3 a, IVec3 b);
IVec3 VectorsUtils_IVec3_Add(const IVec3 a, const IVec3 b);
IVec3 VectorUtils_2DTo3D(IVec2 vector, Axis axis);
double VectorUtils_IVec3_Distance(IVec3 a, IVec3 b);
IVec2 VectorUtils_IVec2_Substract(const IVec2 a, const IVec2 b);
IVec3 VectorUtils_IVec3_Substract(const IVec3 a, const IVec3 b);
int VectorUtils_IVec3_DotProduct(const IVec3 a, const IVec3 b);
int VectorUtils_IVec2_DotProduct(const IVec2 a, const IVec2 b);
FVec3 VectorUtils_FVec3_ScalarMultiply(const FVec3 vector, float t);
FVec3 VectorUtils_FVec3_Add(const FVec3 a, const FVec3 b);
FVec3 VectorUtils_FVec3_Substract(const FVec3 a, const FVec3 b);
float VectorUtils_FVec3_DotProduct(const FVec3 a, const FVec3 b);
FVec3 VectorUtils_IVec3_ConvertFVec3(const IVec3 a);
IVec3 VectorUtils_FVec3_ConvertIVec3(const FVec3 a);
float VectorUtils_FVec3_Distance(const FVec3 a, const FVec3 b);

#endif

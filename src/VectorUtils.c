#include <math.h>

#include "VectorUtils.h"

IVec3 VectorUtils_IVec3_Min(IVec3 a, IVec3 b) {
    IVec3 result;

    result.X = (a.X <= b.X) ? a.X : b.X;
    result.Y = (a.Y <= b.Y) ? a.Y : b.Y;
    result.Z = (a.Z <= b.Z) ? a.Z : b.Z;

    return result;
}

IVec3 VectorUtils_IVec3_Max(IVec3 a, IVec3 b) {
    IVec3 result;

    result.X = (a.X >= b.X) ? a.X : b.X;
    result.Y = (a.Y >= b.Y) ? a.Y : b.Y;
    result.Z = (a.Z >= b.Z) ? a.Z : b.Z;

    return result;
}

IVec3 VectorUtils_2DTo3D(IVec2 vector, Axis axis) {
    IVec3 result;
    switch (axis) {
        case AXIS_X:
            result.X = 0;
            result.Y = vector.X;
            result.Z = vector.Y; 
            return result;
        case AXIS_Y:
            result.X = vector.X;
            result.Y = 0;
            result.Z = vector.Y; 
            return result;
        case AXIS_Z:
        default:
            result.X = vector.X;
            result.Y = vector.Y;
            result.Z = 0;
            return result;
    }
}

IVec3 VectorsUtils_IVec3_Add(const IVec3 a, const IVec3 b) {
    IVec3 result;
    result.X = a.X + b.X;
    result.Y = a.Y + b.Y;
    result.Z = a.Z + b.Z;
    return result;
}

IVec3 VectorUtils_IVec3_Substract(const IVec3 a, const IVec3 b) {
    IVec3 result;
    result.X = a.X - b.X;
    result.Y = a.Y - b.Y;
    result.Z = a.Z - b.Z;
    return result;
}

int VectorUtils_IVec3_DotProduct(const IVec3 a, const IVec3 b) {
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

int VectorUtils_IVec2_DotProduct(const IVec2 a, const IVec2 b) {
    return a.X * b.X + a.Y * b.Y;
}

double VectorUtils_IVec3_Distance(IVec3 a, IVec3 b) {
    IVec3 difference = VectorUtils_IVec3_Substract(a, b);
    return sqrt(VectorUtils_IVec3_DotProduct(difference, difference));
}

IVec2 VectorUtils_IVec2_Substract(const IVec2 a, const IVec2 b) {
    IVec2 result = {
        .X = a.X - b.X,
        .Y = a.Y - b.Y,
    };

    return result;
}

FVec3 VectorUtils_FVec3_ScalarMultiply(const FVec3 vector, float t) {
    FVec3 result = {
        .X = vector.X * t,
        .Y = vector.Y * t,
        .Z = vector.Z * t
    };

    return result;
}

FVec3 VectorUtils_FVec3_Add(const FVec3 a, const FVec3 b) {
    FVec3 result = {
        .X = a.X + b.X,
        .Y = a.Y + b.Y,
        .Z = a.Z + b.Z
    };

    return result;
}

FVec3 VectorUtils_FVec3_Substract(const FVec3 a, const FVec3 b) {
    FVec3 result = {
        .X = a.X - b.X,
        .Y = a.Y - b.Y,
        .Z = a.Z - b.Z
    };

    return result;
}

FVec3 VectorUtils_IVec3_ConvertFVec3(const IVec3 a) {
    FVec3 result = {
        (float)a.X,
        (float)a.Y,
        (float)a.Z
    };

    return result;
}

IVec3 VectorUtils_FVec3_ConvertIVec3(const FVec3 a) {
    IVec3 result = {
        round(a.X),
        round(a.Y),
        round(a.Z)
    };

    return result;
}

float VectorUtils_FVec3_DotProduct(const FVec3 a, const FVec3 b) {
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

float VectorUtils_FVec3_Distance(const FVec3 a, const FVec3 b) {
    FVec3 difference = VectorUtils_FVec3_Substract(a, b);
    return sqrt(VectorUtils_FVec3_DotProduct(difference, difference));
}

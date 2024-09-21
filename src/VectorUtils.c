#include <math.h>

#include "VectorUtils.h"

IVec3 VectorUtils_IVec3_Min(IVec3 a, IVec3 b) {
    IVec3 result;

    result.x = (a.x <= b.x) ? a.x : b.x;
    result.y = (a.y <= b.y) ? a.y : b.y;
    result.z = (a.z <= b.z) ? a.z : b.z;

    return result;
}

IVec3 VectorUtils_IVec3_Max(IVec3 a, IVec3 b) {
    IVec3 result;

    result.x = (a.x >= b.x) ? a.x : b.x;
    result.y = (a.y >= b.y) ? a.y : b.y;
    result.z = (a.z >= b.z) ? a.z : b.z;

    return result;
}

IVec3 VectorUtils_2DTo3D(IVec2 vector, Axis axis) {
    IVec3 result;
    switch (axis) {
        case AXIS_X:
            result.x = 0;
            result.y = vector.x;
            result.z = vector.y; 
            return result;
        case AXIS_Y:
            result.x = vector.x;
            result.y = 0;
            result.z = vector.y; 
            return result;
        case AXIS_Z:
        default:
            result.x = vector.x;
            result.y = vector.y;
            result.z = 0;
            return result;
    }
}

IVec3 VectorsUtils_IVec3_Add(const IVec3 a, const IVec3 b) {
    IVec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

IVec3 VectorUtils_IVec3_Substract(const IVec3 a, const IVec3 b) {
    IVec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

int VectorUtils_IVec3_DotProduct(const IVec3 a, const IVec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int VectorUtils_IVec2_DotProduct(const IVec2 a, const IVec2 b) {
    return a.x * b.x + a.y * b.y;
}

double VectorUtils_IVec3_Distance(IVec3 a, IVec3 b) {
    IVec3 difference = VectorUtils_IVec3_Substract(a, b);
    return sqrt(VectorUtils_IVec3_DotProduct(difference, difference));
}

IVec2 VectorUtils_IVec2_Substract(const IVec2 a, const IVec2 b) {
    IVec2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };

    return result;
}

FVec3 VectorUtils_FVec3_ScalarMultiply(const FVec3 vector, float t) {
    FVec3 result = {
        .x = vector.x * t,
        .y = vector.y * t,
        .z = vector.z * t
    };

    return result;
}

FVec3 VectorUtils_FVec3_Add(const FVec3 a, const FVec3 b) {
    FVec3 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };

    return result;
}

FVec3 VectorUtils_FVec3_Substract(const FVec3 a, const FVec3 b) {
    FVec3 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };

    return result;
}

FVec3 VectorUtils_IVec3_ConvertFVec3(const IVec3 a) {
    FVec3 result = {
        (float)a.x,
        (float)a.y,
        (float)a.z
    };

    return result;
}

IVec3 VectorUtils_FVec3_ConvertIVec3(const FVec3 a) {
    IVec3 result = {
        round(a.x),
        round(a.y),
        round(a.z)
    };

    return result;
}

float VectorUtils_FVec3_DotProduct(const FVec3 a, const FVec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float VectorUtils_FVec3_Distance(const FVec3 a, const FVec3 b) {
    FVec3 difference = VectorUtils_FVec3_Substract(a, b);
    return sqrt(VectorUtils_FVec3_DotProduct(difference, difference));
}

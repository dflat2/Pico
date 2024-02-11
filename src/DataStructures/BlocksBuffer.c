#include <stdlib.h>

#include "ClassiCube/src/World.h"

#include "BlocksBuffer.h"
#include "VectorsExtension.h"
#include "Memory.h"

typedef enum S3_ { XYZ, XZY, ZYX, YXZ, ZXY, YZX } S3;

typedef struct Transform_ {
    S3 permutation;
    bool flipX;
    bool flipY;
    bool flipZ;
} Transform;

static BlocksBuffer s_Buffer = { 0 };
static bool s_BufferIsEmpty = true;

static const Transform RotateX90 = { XZY, .flipX = false, .flipY = false, .flipZ = true };
static const Transform RotateX180 = { XYZ, .flipX = false, .flipY = true, .flipZ = true };
static const Transform RotateX270 = { XZY, .flipX = false, .flipY = true, .flipZ = false };
static const Transform RotateY90 = { ZYX, .flipX = true, .flipY = false, .flipZ = false };
static const Transform RotateY180 = { XYZ, .flipX = true, .flipY = false, .flipZ = true };
static const Transform RotateY270 = { ZYX, .flipX = false, .flipY = false, .flipZ = true };
static const Transform RotateZ90 = { YXZ, .flipX = false, .flipY = true, .flipZ = false };
static const Transform RotateZ180 = { XYZ, .flipX = true, .flipY = true, .flipZ = false };
static const Transform RotateZ270 = { YXZ, .flipX = true, .flipY = false, .flipZ = false };
static const Transform FlipX = { XYZ, .flipX = true, .flipY = false, .flipZ = false };
static const Transform FlipY = { XYZ, .flipX = false, .flipY = true, .flipZ = false };
static const Transform FlipZ = { XYZ, .flipX = false, .flipY = false, .flipZ = true };

BlocksBuffer BlocksBuffer_GetCopied(void) {
    return s_Buffer;
}

bool BlocksBuffer_IsEmpty(void) {
    return s_BufferIsEmpty;
}

bool BlocksBuffer_Copy_MALLOC(IVec3 mark1, IVec3 mark2, int* out_amountCopied) {
    IVec3 min = Min(mark1, mark2);
    IVec3 max = Max(mark1, mark2);
    IVec3 anchor = Substract(mark1, min);

    int width = max.X - min.X + 1;
    int height = max.Y - min.Y + 1;
    int length = max.Z - min.Z + 1;

    BlockID* blocks = Memory_AllocateZeros(width * height * length, sizeof(BlockID));

    if (Memory_AllocationError()) {
        return false;
    }

    int index = 0;

    for (int x = min.X; x <= max.X; x++) {
        for (int y = min.Y; y <= max.Y; y++) {
            for (int z = min.Z; z <= max.Z; z++) {
                blocks[index] = World_GetBlock(x, y, z);
                index++;
            }
        }
    }

    s_Buffer.dimensions.X = max.X - min.X + 1;
    s_Buffer.dimensions.Y = max.Y - min.Y + 1;
    s_Buffer.dimensions.Z = max.Z - min.Z + 1;
    s_Buffer.anchor = anchor;

    if (!s_BufferIsEmpty) {
        free(s_Buffer.content);
    }

    s_Buffer.content = blocks;
    s_BufferIsEmpty = false;
    *out_amountCopied = width * height * length;
    return true;
}

static IVec3 ApplyPermutation(S3 permutation, IVec3 vector) {
    IVec3 result = { .X = vector.X, .Y = vector.Y, .Z = vector.Z };

    switch (permutation) {
        case XYZ:
            break;
        case XZY:
            result.Y = vector.Z;
            result.Z = vector.Y;
            break;
        case YXZ:
            result.X = vector.Y;
            result.Y = vector.X;
            break;
        case ZYX:
            result.X = vector.Z;
            result.Z = vector.X;
            break;
        case ZXY:
            result.X = vector.Z;
            result.Y = vector.X;
            result.Z = vector.Y;
            break;
        case YZX:
            result.X = vector.Y;
            result.Y = vector.Z;
            result.Z = vector.X;
            break;
        default:
            break;
    }

    return result;
}

static IVec3 ApplyTransform(Transform transform, IVec3 coordinates, IVec3 previousDimensions) {
    if (transform.flipX) {
        coordinates.X = previousDimensions.X - 1 - coordinates.X;
    }

    if (transform.flipY) {
        coordinates.Y = previousDimensions.Y - 1 - coordinates.Y;
    }

    if (transform.flipZ) {
        coordinates.Z = previousDimensions.Z - 1 - coordinates.Z;
    }

    coordinates = ApplyPermutation(transform.permutation, coordinates);
    return coordinates;
}

static int Pack(IVec3 vector) {
    return vector.Z + (vector.Y * s_Buffer.dimensions.Z) + vector.X * (s_Buffer.dimensions.Y * s_Buffer.dimensions.Z);
}

static bool TransformBuffer_MALLOC(Transform transform) {
    BlockID* newContent = Memory_Allocate(sizeof(BlockID) * s_Buffer.dimensions.X * s_Buffer.dimensions.Y * s_Buffer.dimensions.Z);

    if (Memory_AllocationError()) {
        return false;
    }

    IVec3 previousDimensions = s_Buffer.dimensions;
    s_Buffer.dimensions = ApplyPermutation(transform.permutation, s_Buffer.dimensions);

    IVec3 coordinates;
    IVec3 transformed;
    int index = 0;

    for (coordinates.X = 0; coordinates.X < previousDimensions.X; coordinates.X++) {
        for (coordinates.Y = 0; coordinates.Y < previousDimensions.Y; coordinates.Y++) {
            for (coordinates.Z = 0; coordinates.Z < previousDimensions.Z; coordinates.Z++) {
                transformed = ApplyTransform(transform, coordinates, previousDimensions);
                newContent[Pack(transformed)] = s_Buffer.content[index];
                index++;
            }
        }
    }

    free(s_Buffer.content);
    s_Buffer.content = newContent;
    s_Buffer.anchor = ApplyTransform(transform, s_Buffer.anchor, previousDimensions);
    return true;
}

bool BlocksBuffer_Rotate_MALLOC(Axis axis, int count) {
    count = count % 4;

    if (count == 0) {
        return false;
    }

    Transform transform = RotateY90;

    if (axis == AXIS_X) {
        if (count == 1) {
            transform = RotateX90;
        } else if (count == 2) {
            transform = RotateX180;
        } else {
            transform = RotateX270;
        }
    } else if (axis == AXIS_Y) {
        if (count == 1) {
            transform = RotateY90;
        } else if (count == 2) {
            transform = RotateY180;
        } else {
            transform = RotateY270;
        }
    } else if (axis == AXIS_Z) {
        if (count == 1) {
            transform = RotateZ90;
        } else if (count == 2) {
            transform = RotateZ180;
        } else {
            transform = RotateZ270;
        }
    }

    return TransformBuffer_MALLOC(transform);
}

bool BlocksBuffer_Flip_MALLOC(Axis axis) {
    if (axis == AXIS_X) {
        return TransformBuffer_MALLOC(FlipX);
    } else if (axis == AXIS_Y) {
        return TransformBuffer_MALLOC(FlipY);
    } else if (axis == AXIS_Z) {
        return TransformBuffer_MALLOC(FlipZ);
    }

    return false;
}

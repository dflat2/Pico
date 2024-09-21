#include <stdlib.h>

#include "ClassiCube/src/World.h"

#include "BlocksBuffer.h"
#include "VectorUtils.h"
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

bool BlocksBuffer_Copy(IVec3 mark1, IVec3 mark2, int* out_amountCopied) {
    IVec3 min = VectorUtils_IVec3_Min(mark1, mark2);
    IVec3 max = VectorUtils_IVec3_Max(mark1, mark2);
    IVec3 anchor = VectorUtils_IVec3_Substract(mark1, min);

    int width = max.x - min.x + 1;
    int height = max.y - min.y + 1;
    int length = max.z - min.z + 1;

    BlockID* blocks = Memory_AllocateZeros(width * height * length, sizeof(BlockID));

    int index = 0;

    for (int x = min.x; x <= max.x; x++) {
        for (int y = min.y; y <= max.y; y++) {
            for (int z = min.z; z <= max.z; z++) {
                blocks[index] = World_GetBlock(x, y, z);
                index++;
            }
        }
    }

    s_Buffer.dimensions.x = max.x - min.x + 1;
    s_Buffer.dimensions.y = max.y - min.y + 1;
    s_Buffer.dimensions.z = max.z - min.z + 1;
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
    IVec3 result = { .x = vector.x, .y = vector.y, .z = vector.z };

    switch (permutation) {
        case XYZ:
            break;
        case XZY:
            result.y = vector.z;
            result.z = vector.y;
            break;
        case YXZ:
            result.x = vector.y;
            result.y = vector.x;
            break;
        case ZYX:
            result.x = vector.z;
            result.z = vector.x;
            break;
        case ZXY:
            result.x = vector.z;
            result.y = vector.x;
            result.z = vector.y;
            break;
        case YZX:
            result.x = vector.y;
            result.y = vector.z;
            result.z = vector.x;
            break;
        default:
            break;
    }

    return result;
}

static IVec3 ApplyTransform(Transform transform, IVec3 coordinates, IVec3 previousDimensions) {
    if (transform.flipX) {
        coordinates.x = previousDimensions.x - 1 - coordinates.x;
    }

    if (transform.flipY) {
        coordinates.y = previousDimensions.y - 1 - coordinates.y;
    }

    if (transform.flipZ) {
        coordinates.z = previousDimensions.z - 1 - coordinates.z;
    }

    coordinates = ApplyPermutation(transform.permutation, coordinates);
    return coordinates;
}

static int Pack(IVec3 vector) {
    return vector.z + (vector.y * s_Buffer.dimensions.z) + vector.x * (s_Buffer.dimensions.y * s_Buffer.dimensions.z);
}

static bool TransformBuffer(Transform transform) {
    BlockID* newContent = Memory_Allocate(sizeof(BlockID) * s_Buffer.dimensions.x * s_Buffer.dimensions.y * s_Buffer.dimensions.z);

    IVec3 previousDimensions = s_Buffer.dimensions;
    s_Buffer.dimensions = ApplyPermutation(transform.permutation, s_Buffer.dimensions);

    IVec3 coordinates;
    IVec3 transformed;
    int index = 0;

    for (coordinates.x = 0; coordinates.x < previousDimensions.x; coordinates.x++) {
        for (coordinates.y = 0; coordinates.y < previousDimensions.y; coordinates.y++) {
            for (coordinates.z = 0; coordinates.z < previousDimensions.z; coordinates.z++) {
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

bool BlocksBuffer_Rotate(Axis axis, int count) {
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

    return TransformBuffer(transform);
}

bool BlocksBuffer_Flip(Axis axis) {
    if (axis == AXIS_X) {
        return TransformBuffer(FlipX);
    } else if (axis == AXIS_Y) {
        return TransformBuffer(FlipY);
    } else if (axis == AXIS_Z) {
        return TransformBuffer(FlipZ);
    }

    return false;
}

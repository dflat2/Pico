#ifndef BLOCKS_REGISTER_H
#define BLOCKS_REGISTER_H

#include <CC_API/Core.h>

typedef enum Axis_
{
    X, Y, Z
} Axis;

typedef struct SignedAxis_
{
    Axis axis;
    bool negative;
} SignedAxis;

typedef struct BlocksRegister_
{
    int Width, Height, Length;
    BlockID* Blocks;
    int OriginX, OriginY, OriginZ;
} BlocksRegister;

void Rotate(BlocksRegister* blocksRegister, Axis axis, int angle);
void Flip(BlocksRegister* blocksRegister, Axis axis);

#endif /* BLOCKS_REGISTER_H */
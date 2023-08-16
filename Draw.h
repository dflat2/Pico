#ifndef DRAW_H
#define DRAW_H

#include "CC_API/Block.h"

void Draw_Start(char* description);
void Draw_Block(short x, short y, short z, BlockID block);
int Draw_End();

#endif /* DRAW_H */

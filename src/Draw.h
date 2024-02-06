#ifndef DRAW_H
#define DRAW_H

#include "ClassiCube/src/Block.h"

#include "Brushes/Brush.h"

void Draw_Start(char* description);
void Draw_Block(int x, int y, int z, BlockID block);
void Draw_Brush(int x, int y, int z); 
int Draw_End(void);

#endif

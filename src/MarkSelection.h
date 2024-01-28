#ifndef MARK_SELECTION_H
#define MARK_SELECTION_H

#include "ClassiCube/src/Vectors.h"

#include <stdbool.h>

typedef void (*SelectionHandler)(IVec3* marks, int count);

void MarkSelection_Make(SelectionHandler handler, int count, const char* operation);
void MarkSelection_Abort(void);
void MarkSelection_DoMark(IVec3 coordinates);
int MarkSelection_RemainingMarks(void);

#endif

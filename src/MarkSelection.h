#ifndef MARK_SELECTION_H
#define MARK_SELECTION_H

#include "ClassiCube/src/Vectors.h"

#include <stdbool.h>

typedef void (*SelectionHandler)(IVec3* marks, int count);

void MarkSelection_SetStatic(bool _static);
void MarkSelection_Make(SelectionHandler handler, int count);
void MarkSelection_Abort();
void MarkSelection_DoMark(IVec3 coordinates);
int MarkSelection_RemainingMarks();
void MarkSelection_TogglePaint();
bool MarkSelection_Painting();

#endif /* MARK_SELECTION_H */

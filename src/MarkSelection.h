#ifndef MARK_SELECTION_H
#define MARK_SELECTION_H

#include <stdbool.h>

#define MACRO_MARKSELECTION_DO_REPEAT true
#define MACRO_MARKSELECTION_DO_NOT_REPEAT false

#include "ClassiCube/src/Vectors.h"

typedef void (*SelectionHandler)(IVec3* marks, int count);

void MarkSelection_Make(SelectionHandler handler, int count, const char* operation, bool repeat);
bool MarkSelection_Repeating(void);
void MarkSelection_Abort(void);
void MarkSelection_DoMark(IVec3 coordinates);
int MarkSelection_RemainingMarks(void);

#endif

#ifndef MARK_SELECTION_H
#define MARK_SELECTION_H

#include "CC_API/Vectors.h"
#include <stdbool.h>

typedef void (*SelectionHandler)(IVec3* marks, int count);
typedef void (*ResourceCleaner)(void* object);

void MarkSelection_Make(SelectionHandler handler, int count);
void MarkSelection_Abort();
void MarkSelection_DoMark(IVec3 coordinates);
void MarkSelection_SetStatic(void (*DoCommand)(const cc_string* args, int argsCount), const cc_string* args, int argsCount);
int MarkSelection_RemainingMarks();
void MarkSelection_TogglePaint();
bool MarkSelection_Painting();

#endif /* MARK_SELECTION_H */

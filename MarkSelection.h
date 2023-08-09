#ifndef MARK_SELECTION_H
#define MARK_SELECTION_H

#include "CC_API/Vectors.h"
#include <stdbool.h>

typedef void (*SelectionHandler)(IVec3* marks, int count, void* object);
typedef void (*ResourceCleaner)(void* object);
void MakeSelection(SelectionHandler handler, int count, void* extraParameters, ResourceCleaner resourceCleaner);
void AbortSelection();
void DoMark(IVec3 coordinates);
int RemainingMarks();

#endif /* MARK_SELECTION_H */

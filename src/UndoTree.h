#ifndef UNDO_TREE_H
#define UNDO_TREE_H

#include "ClassiCube/src/Core.h"

#include <stdbool.h>

// DeltaBlockID is a difference between two BlockIDs. Only deltas are saved in each operation.
// e.g. BLOCK_LOG - BLOCK_SAND = 5 may happen if one did /replace sand log.
typedef int DeltaBlockID;

// Initialisation
bool UndoTree_Enable_MALLOC(void);
void UndoTree_Disable(void);

// Undo tree navigation.
bool UndoTree_Undo_MALLOC(void);
bool UndoTree_Earlier_MALLOC(int deltaTime_Second, int* out_commit); 
bool UndoTree_Later_MALLOC(int deltaTime_S, int* commit);
bool UndoTree_Checkout_MALLOC(int commit, int* out_ascended, int* out_descended);
bool UndoTree_Redo_MALLOC(void);

// Attaching a new node.
bool UndoTree_PrepareNewNode_MALLOC(char* description);
void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta);
void UndoTree_Commit(void);

// Information.
void UndoTree_UndoList(cc_string* descriptions, int* descriptionsCount);
void UndoTree_FormatCurrentNode(cc_string* destination);
long UndoTree_CurrentTimestamp(void);

#endif

#ifndef UNDO_TREE_H
#define UNDO_TREE_H

#include <stdbool.h>

// DeltaBlockID is a difference between two BlockIDs. Only deltas are saved in each operation.
// e.g. BLOCK_LOG - BLOCK_SAND = 5 may happen if one did /replace sand log.
typedef int DeltaBlockID;

// Initialisation and cleaning.
void UndoTree_Enable();
void UndoTree_Disable();

// Undo tree navigation.
bool UndoTree_Ascend();
bool UndoTree_Descend();
void UndoTree_Earlier(int deltaTime_Second, int* ascended, int* descended);
void UndoTree_Later(int deltaTime_Second, int* ascended, int* descended);
bool UndoTree_Checkout(int commit, int* out_ascended, int* out_descended);
bool UndoTree_Redo();

// Attaching a new node.
void UndoTree_PrepareNewNode(char* description);
void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta);
void UndoTree_Commit();

// Information.
void UndoTree_ShowLeaves();
bool UndoTree_Enabled();
long UndoTree_CurrentTimestamp_Millisecond();

#endif

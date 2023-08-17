#ifndef UNDO_TREE_H
#define UNDO_TREE_H

#include <stdbool.h>

// DeltaBlockID is a difference between two BlockIDs. Only deltas are saved in each operation.
// e.g. BLOCK_LOG - BLOCK_SAND = 5 may happen if one did /replace sand log.
typedef signed short DeltaBlockID;

// Initialisation and cleaning.
void UndoTree_Enable();
void UndoTree_Disable();

// Undo tree navigation.
bool UndoTree_Ascend();
bool UndoTree_Descend();
void UndoTree_Earlier_Second(int deltaTime_Second);
void UndoTree_Later_Second(int deltaTime_Second);
void UndoTree_Earlier(int count);
void UndoTree_Later(int count);
void UndoTree_Checkout(int commit);
bool UndoTree_Redo();

// Attaching a new node.
void UndoTree_PrepareNewNode(char* description);
void UndoTree_AddBlockChangeEntry(signed short x, signed short y, signed short z, DeltaBlockID delta);
void UndoTree_Commit();

// Information.
void UndoTree_ShowLeaves();
bool UndoTree_Enabled();

#endif

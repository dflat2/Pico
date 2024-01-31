#ifndef UNDO_TREE_H
#define UNDO_TREE_H

#include <stdlib.h>
#include <stdbool.h>

// DeltaBlockID is a difference between two BlockIDs. Only deltas are saved in each operation.
// e.g. BLOCK_LOG - BLOCK_SAND = 5 may happen if one did /replace sand log.
typedef int DeltaBlockID;

// Initialisation
bool UndoTree_Enable(void);
void UndoTree_Disable(void);

// Undo tree navigation.
bool UndoTree_Undo(void);
bool UndoTree_Earlier(int deltaTime_Second, int* out_commit); 
bool UndoTree_Later(int deltaTime_S, int* commit);
bool UndoTree_Checkout(int commit, int* out_ascended, int* out_descended);
bool UndoTree_Redo(void);

// Attaching a new node.
bool UndoTree_TryPrepareNewNode(char* description);
void UndoTree_AddBlockChangeEntry(int x, int y, int z, DeltaBlockID delta);
void UndoTree_Commit(void);

// Information.
void UndoTree_UndoList(cc_string* descriptions, int* descriptionsCount);
void UndoTree_FormatCurrentNode(cc_string* destination);
long UndoTree_CurrentTimestamp(void);

#endif

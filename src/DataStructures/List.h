#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct List_ List;

void* List_Get(List* list, int index);
void* List_Pop(List* list);
int List_IndexOf(List* list, void* data);
bool List_Contains(List* list, void* data);
int List_Count(List* list);
bool List_IsEmpty(List* list);
List* List_CreateEmpty(void);
void List_Append(List* list, void* data);
void List_Clear(List* list);
void List_Free(List* list);

#endif

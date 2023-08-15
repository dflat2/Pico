#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "List.h"

typedef struct ListElement_ ListElement;

typedef struct ListElement_ {
    void* data;
    ListElement* next;
    ListElement* previous;
} ListElement;

typedef struct List_ {
    ListElement* firstElement;
} List;

void* List_Get(List* list, int index) {
    ListElement* current = list->firstElement;

    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    return current->data;
}

void* List_Pop(List* list) {
    if (list->firstElement == NULL) {
        return NULL;
    }

    ListElement* lastElement = list->firstElement;
    while (lastElement->next) {
        lastElement = lastElement->next;
    }

    if (lastElement->previous) {
        lastElement->previous->next = NULL;
    } else {
        list->firstElement = NULL;
    }

    void* data = lastElement->data;
    free(lastElement);
    return data;
}

int List_IndexOf(List* list, void* data) {
    ListElement* current = list->firstElement;
    int index = 0;

    while (current) {
        if (current->data == data) {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1;
}

bool List_Contains(List* list, void* data) {
    return List_IndexOf(list, data) != -1;
}

int List_Count(List* list) {
    int count = 0;
    ListElement* current = list->firstElement;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

bool List_IsEmpty(List* list) {
    return list->firstElement == NULL;
}

List* List_CreateEmpty() {
    List* list = (List*)malloc(sizeof(List));
    list->firstElement = NULL;
    return list;
}

void List_Append(List* list, void* data) {
    ListElement* newElement = (ListElement*)malloc(sizeof(ListElement));
    newElement->data = data;
    newElement->next = NULL;
    newElement->previous = NULL;
    
    if (list->firstElement == NULL) {
        list->firstElement = newElement;
    } else {
        ListElement* current = list->firstElement;
        while (current->next) {
            current = current->next;
        }
        current->next = newElement;
        newElement->previous = current;
    }
}

void List_Clear(List* list) {
    while (!List_IsEmpty(list)) {
        List_Pop(list);
    }
}

void List_Free(List* list) {
    List_Clear(list);
    free(list);
}

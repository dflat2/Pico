#include <stdio.h>

#include "CC_API/Event.h"
#include "CC_API/Game.h"

#include "MarkSelection.h"
#include "Messaging.h"
#include "MemoryAllocation.h"

static bool s_InProgress = false;
static int s_CurrentMark = 0;
static int s_TotalMarks = 0;
static IVec3* s_Marks = NULL;
static SelectionHandler s_Handler = NULL;
static void* s_ExtraParameters = NULL;
static ResourceCleaner s_ResourceCleaner = NULL;

static void ValidateSelection();

void MarkSelection_DoMark(IVec3 coords) {
    if (!s_InProgress) {
        Message_Player("&fCannot mark, no selection in progress.");
        return;
    }

    s_Marks[s_CurrentMark] = coords;
    s_CurrentMark++;

    char message[64];
    snprintf(message, 64, "&fMark placed at &b(%d, %d, %d)&f.", coords.X, coords.Y, coords.Z);
    Message_Player(message);

    if (s_CurrentMark == s_TotalMarks) {
        ValidateSelection();
        return;
    }
}

static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock) {
    Game_UpdateBlock(coords.X, coords.Y, coords.Z, oldBlock);
    MarkSelection_DoMark(coords);
}

static void RegisterBlockChanged() {
    Event_Register((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void UnregisterBlockChanged() {
    Event_Unregister((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void CallHandler() {
    s_Handler(s_Marks, s_TotalMarks, s_ExtraParameters);
}

static void FreeResources() {
    free(s_Marks);

    if (s_ResourceCleaner != NULL) {
        ((ResourceCleaner) s_ResourceCleaner)(s_ExtraParameters);
    }
}

static void ResetSelectionState() {
    s_InProgress = false;
    s_CurrentMark = 0;
    s_TotalMarks = 0;
    s_Marks = NULL;
    s_Handler = NULL;
    s_ExtraParameters = NULL;
    s_ResourceCleaner = NULL;
}

void MarkSelection_Abort() {
    FreeResources();
    ResetSelectionState();
    UnregisterBlockChanged();
}

static void ValidateSelection() {
    CallHandler();
    MarkSelection_Abort();
}

int MarkSelection_RemainingMarks() {
    if (!s_InProgress) {
        return 0;
    }

    return s_TotalMarks - s_CurrentMark;
}

void MarkSelection_Make(SelectionHandler handler, int count, void* extraParameters, ResourceCleaner resourceCleaner) {
    if (s_InProgress) {
        MarkSelection_Abort();
    }

    s_InProgress = true;
    s_CurrentMark = 0;
    s_TotalMarks = count;
    s_Marks = allocate(count, sizeof(IVec3));
    s_Handler = handler;
    s_ExtraParameters = extraParameters;
    s_ResourceCleaner = resourceCleaner;
    RegisterBlockChanged();
}

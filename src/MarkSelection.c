#include <stdio.h>

#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Inventory.h"

#include "MarkSelection.h"
#include "Messaging.h"
#include "Draw.h"

#define MAX_MARKS 4

static bool s_InProgress = false;
static int s_CurrentMark = 0;
static int s_TotalMarks = 0;
static IVec3 s_Marks[MAX_MARKS] = { 0 };
static SelectionHandler s_Handler = NULL;

static void ValidateSelection();
static void ResetSelectionState();
static void UnregisterBlockChanged();
static void RegisterBlockChanged();
static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock);

void MarkSelection_DoMark(IVec3 coords) {
    if (!s_InProgress) {
        Message_Player("Cannot mark, no selection in progress.");
        return;
    }

    s_Marks[s_CurrentMark] = coords;
    s_CurrentMark++;

    char message[64];
    snprintf(message, 64, "&fMark placed at &b(%d, %d, %d)&f.", coords.X, coords.Y, coords.Z);
    Message_Player(message);

    if (s_CurrentMark == s_TotalMarks) {
        ValidateSelection();
    }
}

void MarkSelection_Abort() {
    ResetSelectionState();
    UnregisterBlockChanged();
}

int MarkSelection_RemainingMarks() {
    if (!s_InProgress) {
        return 0;
    }

    return s_TotalMarks - s_CurrentMark;
}

void MarkSelection_Make(SelectionHandler handler, int count) {
	if (s_InProgress) {
		MarkSelection_Abort();
	}

    s_InProgress = true;
    s_CurrentMark = 0;
    s_TotalMarks = count;
    s_Handler = handler;
    RegisterBlockChanged();
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

static void ResetSelectionState() {
    s_InProgress = false;
    s_CurrentMark = 0;
    s_TotalMarks = 0;
    s_Handler = NULL;
}

static void ValidateSelection() {
	UnregisterBlockChanged();
	s_InProgress = false;
    s_Handler(s_Marks, s_TotalMarks);
}
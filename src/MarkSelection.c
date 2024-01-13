#include <stdio.h>

#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Inventory.h"

#include "MarkSelection.h"
#include "Messaging.h"
#include "MemoryAllocation.h"
#include "SPCCommand.h"
#include "Draw.h"

static bool s_InProgress = false;
static int s_CurrentMark = 0;
static int s_TotalMarks = 0;
static IVec3* s_Marks = NULL;
static SelectionHandler s_Handler = NULL;
static void (*s_StaticCommand)(const cc_string* arguments, int argumentsCount);
static cc_string* s_StaticArgs = NULL;
static int s_StaticArgsCount = 0;
static bool s_Painting = false;

static void ValidateSelection();
static void ResetSelectionState();
static void FreeResources();
static void CallHandler();
static void UnregisterBlockChanged();
static void RegisterBlockChanged();
static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock);
static void FreeStatic();
static void FreeMarks();
static void CallStaticFunction();
static void ShowMode(const char* mode);

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
    }

}

void MarkSelection_Abort() {
    FreeResources();
    ResetSelectionState();
    UnregisterBlockChanged();
	ShowMode("Normal");
}

int MarkSelection_RemainingMarks() {
    if (!s_InProgress) {
        return 0;
    }

    return s_TotalMarks - s_CurrentMark;
}

void MarkSelection_SetStatic(void (*DoCommand)(const cc_string* args, int argsCount), const cc_string* args, int argsCount) {
	MarkSelection_Abort();
	s_StaticCommand = DoCommand;
	
	if (argsCount != 0) {
		s_StaticArgs = allocateZeros(argsCount, sizeof(cc_string));
	} else {
		s_StaticArgs = NULL;
	}

	for (int i = 0; i < argsCount; i++) {
		s_StaticArgs[i].buffer = allocate(64, sizeof(char));
		s_StaticArgs[i].capacity = 64;
		String_Copy(&s_StaticArgs[i], &args[i]);
	}
	
	s_StaticArgsCount = argsCount;
	ShowMode("Static");
	CallStaticFunction();
}

void MarkSelection_Make(SelectionHandler handler, int count) {
	if (s_InProgress || s_Painting) {
		MarkSelection_Abort();
	}

    s_InProgress = true;
    s_CurrentMark = 0;
    s_TotalMarks = count;
    s_Marks = allocate(count, sizeof(IVec3));
    s_Handler = handler;
    RegisterBlockChanged();
}

void MarkSelection_TogglePaint() {
	if (s_Painting) {
		MarkSelection_Abort();
		return;
	}

	MarkSelection_Abort();
	s_Painting = true;
	RegisterBlockChanged();
	Message_Player("Mode: &bPaint&f.");
	ShowMode("Paint");
}

bool MarkSelection_Painting() {
	return s_Painting;
}

static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock) {
	if (s_InProgress) {
		Game_UpdateBlock(coords.X, coords.Y, coords.Z, oldBlock);
		MarkSelection_DoMark(coords);
	} else if (s_Painting && newBlock == BLOCK_AIR) {
		Draw_Start("Paint");
		Game_UpdateBlock(coords.X, coords.Y, coords.Z, oldBlock);
		Draw_Block(coords.X, coords.Y, coords.Z, Inventory_SelectedBlock);
		Draw_End();
	} else if (s_Painting) {
		Draw_Start("Place");
		Game_UpdateBlock(coords.X, coords.Y, coords.Z, oldBlock);
		Draw_Block(coords.X, coords.Y, coords.Z, newBlock);
		Draw_End();
	}
}

static void RegisterBlockChanged() {
    Event_Register((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void UnregisterBlockChanged() {
    Event_Unregister((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void CallHandler() {
    s_Handler(s_Marks, s_TotalMarks);
}

static void FreeStatic() {
	if (s_StaticCommand == NULL) {
		return;
	}

	for (int i = 0; i < s_StaticArgsCount; i++) {
		free(s_StaticArgs[i].buffer);
	}

	free(s_StaticArgs);
	s_StaticArgs = NULL;
	s_StaticCommand = NULL;
}

static void FreeMarks() {
	free(s_Marks);
	s_Marks = NULL;
}

static void FreeResources() {
	FreeMarks();
	FreeStatic();
}

static void ResetSelectionState() {
    s_InProgress = false;
    s_CurrentMark = 0;
    s_TotalMarks = 0;
    s_Marks = NULL;
    s_Handler = NULL;
	s_Painting = false;
}

static void ValidateSelection() {
    CallHandler();
	FreeMarks();
    ResetSelectionState();
    UnregisterBlockChanged();

	if (s_StaticCommand != NULL) {
		CallStaticFunction();
	}
}

static void CallStaticFunction() {
	if (s_StaticCommand == NULL) {
		return;
	}

	s_StaticCommand(s_StaticArgs, s_StaticArgsCount);
}

static void ShowMode(const char* mode) {
	cc_string cc_mode = String_FromReadonly(mode);
	char buffer[64];
	cc_string message = String_FromArray(buffer);
	String_AppendConst(&message, "Mode: &b");
	String_AppendString(&message, &cc_mode);
	Chat_AddOf(&message, MSG_TYPE_BOTTOMRIGHT_1);
}

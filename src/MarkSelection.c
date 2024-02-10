#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Game.h"

#include "MarkSelection.h"
#include "Message.h"

#define MAX_MARKS 4

static bool s_InProgress = false;
static int s_CurrentMark = 0;
static int s_TotalMarks = 0;
static char s_OperationBuffer[64];
static cc_string s_Operation = String_FromArray(s_OperationBuffer);
static IVec3 s_Marks[MAX_MARKS] = { 0 };
static SelectionHandler s_Handler = NULL;

static void ValidateSelection(void);
static void ResetSelectionState(void);
static void ShowMarksLeft(void);
static void UnregisterBlockChanged(void);
static void RegisterBlockChanged(void);
static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock);

void MarkSelection_DoMark(IVec3 coords) {
    if (!s_InProgress) {
        Message_Player("Cannot mark, no selection in progress.");
        return;
    }

    s_Marks[s_CurrentMark] = coords;
    s_CurrentMark++;
    ShowMarksLeft();

    if (s_CurrentMark == s_TotalMarks) {
        ValidateSelection();
    }
}

void MarkSelection_Abort(void) {
    ResetSelectionState();
    UnregisterBlockChanged();
    ShowMarksLeft();
}

int MarkSelection_RemainingMarks(void) {
    if (!s_InProgress) {
        return 0;
    }

    return s_TotalMarks - s_CurrentMark;
}

void MarkSelection_Make(SelectionHandler handler, int count, const char* operation) {
    if (s_InProgress) {
        MarkSelection_Abort();
    }

    cc_string cc_operation = String_FromReadonly(operation);
    String_Copy(&s_Operation, &cc_operation);

    s_InProgress = true;
    s_CurrentMark = 0;
    s_TotalMarks = count;
    s_Handler = handler;
    ShowMarksLeft();
    RegisterBlockChanged();
}

static void ShowMarksLeft(void) {
    int marksLeft = s_TotalMarks - s_CurrentMark;

    if (marksLeft == 0) {
        cc_string emptyMessage = String_FromReadonly("");
        Chat_AddOf(&emptyMessage, MSG_TYPE_BOTTOMRIGHT_1);
        return;
    }

    cc_string diamond = String_FromConst("\x04");
    cc_string aquaCode = String_FromConst("&b");
    cc_string whiteCode = String_FromConst("&f");

    char buffer[64];
    cc_string message = String_FromArray(buffer);
    String_AppendString(&message, &aquaCode);
    String_AppendString(&message, &s_Operation);
    String_AppendConst(&message, " &f[");

    for (int i = 0; i < s_TotalMarks; i++) {
        if (i == s_CurrentMark) {
            String_AppendString(&message, &aquaCode);
            String_AppendString(&message, &diamond);
            String_AppendString(&message, &whiteCode);
        } else {
            String_AppendString(&message, &diamond);
        }
    }

    String_AppendConst(&message, "]");

    Chat_AddOf(&message, MSG_TYPE_BOTTOMRIGHT_1);
}

static void BlockChangedCallback(void* object, IVec3 coords, BlockID oldBlock, BlockID newBlock) {
    Game_UpdateBlock(coords.X, coords.Y, coords.Z, oldBlock);
    MarkSelection_DoMark(coords);
}

static void RegisterBlockChanged(void) {
    Event_Register((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void UnregisterBlockChanged(void) {
    Event_Unregister((struct Event_Void*) &UserEvents.BlockChanged, NULL, (Event_Void_Callback)BlockChangedCallback);
}

static void ResetSelectionState(void) {
    s_InProgress = false;
    s_CurrentMark = 0;
    s_TotalMarks = 0;
    s_Handler = NULL;
}

static void ValidateSelection(void) {
    UnregisterBlockChanged();
    s_InProgress = false;
    s_Handler(s_Marks, s_TotalMarks);
}

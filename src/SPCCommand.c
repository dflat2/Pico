#include <stdio.h>

#include "SPCCommand.h"
#include "DataStructures/List.h"
#include "MemoryAllocation.h"

static List* s_Commands = NULL;
static void Register(SPCCommand* command);

extern SPCCommand AbortSPCCommand;
extern SPCCommand AscendSPCCommand;
extern SPCCommand BrushesSPCCommand;
extern SPCCommand CenterSPCCommand;
extern SPCCommand CheckoutSPCCommand;
extern SPCCommand CircleSPCCommand;
extern SPCCommand CopySPCCommand;
extern SPCCommand CutSPCCommand;
extern SPCCommand DescendSPCCommand;
extern SPCCommand EarlierSPCCommand;
extern SPCCommand FillSPCCommand;
extern SPCCommand FlipSPCCommand;
extern SPCCommand LaterSPCCommand;
extern SPCCommand LineSPCCommand;
extern SPCCommand MarkAllSPCCommand;
extern SPCCommand MarkSPCCommand;
extern SPCCommand MeasureSPCCommand;
extern SPCCommand PaintSPCCommand;
extern SPCCommand PasteSPCCommand;
extern SPCCommand PlaceSPCCommand;
extern SPCCommand ReachDistanceSPCCommand;
extern SPCCommand RedoSPCCommand;
extern SPCCommand ReplaceAllSPCCommand;
extern SPCCommand ReplaceNotSPCCommand;
extern SPCCommand ReplaceSPCCommand;
extern SPCCommand RotateSPCCommand;
extern SPCCommand SphereSPCCommand;
extern SPCCommand StaticSPCCommand;
extern SPCCommand TreeSPCCommand;
extern SPCCommand UndoListSPCCommand;
extern SPCCommand UndoSPCCommand;
extern SPCCommand ZSPCCommand;

void SPCCommand_RegisterAll() {
	Register(&AbortSPCCommand);
	Register(&AscendSPCCommand);
	Register(&BrushesSPCCommand);
	Register(&CenterSPCCommand);
	Register(&CheckoutSPCCommand);
	Register(&CircleSPCCommand);
	Register(&CopySPCCommand);
	Register(&CutSPCCommand);
	Register(&DescendSPCCommand);
	Register(&EarlierSPCCommand);
	Register(&FillSPCCommand);
	Register(&FlipSPCCommand);
	Register(&LaterSPCCommand);
	Register(&LineSPCCommand);
	Register(&MarkAllSPCCommand);
	Register(&MarkSPCCommand);
	Register(&MeasureSPCCommand);
	Register(&PaintSPCCommand);
	Register(&PasteSPCCommand);
	Register(&PlaceSPCCommand);
	Register(&ReachDistanceSPCCommand);
	Register(&RedoSPCCommand);
	Register(&ReplaceAllSPCCommand);
	Register(&ReplaceNotSPCCommand);
	Register(&ReplaceSPCCommand);
	Register(&RotateSPCCommand);
	Register(&SphereSPCCommand);
	Register(&StaticSPCCommand);
	Register(&TreeSPCCommand);
	Register(&UndoListSPCCommand);
	Register(&UndoSPCCommand);
	Register(&ZSPCCommand);
}

SPCCommand* SPCCommand_Find(const cc_string* commandName) {
	if (s_Commands == NULL) {
		return NULL;
	}

	int count = List_Count(s_Commands);
	SPCCommand* current;
	cc_string currentName;

	for (int i = 0; i < count; i++) {
		current = (SPCCommand*) List_Get(s_Commands, i);
		currentName = String_FromReadonly(current->chatCommand->name);

		if (String_CaselessEquals(&currentName, commandName)) {
			return current;
		}
	}

	return NULL;
}

static void Register(SPCCommand* command) {
	if (s_Commands == NULL) {
		s_Commands = List_CreateEmpty();
	}

	Commands_Register(command->chatCommand);
	List_Append(s_Commands, command);
}

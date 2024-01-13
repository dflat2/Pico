#include "ClassiCube/src/Chat.h"

#include "SPCCommand.h"
#include "MarkSelection.h"
#include "Messaging.h"

static void Static_Command(const cc_string* args, int argsCount);
static void ShowCouldNotFindCommand(const cc_string* command);
static void ShowCannotStatic(const cc_string* command);

static struct ChatCommand StaticCommand = {
	"Static",
	Static_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Static <command>",
		"&fRepeats the draw command &b<command>&f.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand StaticSPCCommand = {
	.chatCommand = &StaticCommand,
	.canStatic = false
};

static void Static_Command(const cc_string* args, int argsCount) {
	if (argsCount == 0) {
		Message_Player("Usage: &b/Static <command>&f.");
		return;
	}

	SPCCommand* command = SPCCommand_Find(&args[0]);

	if (command == NULL) {
		ShowCouldNotFindCommand(&args[0]);
		return;
	} else if (!command->canStatic) {
		ShowCannotStatic(&args[0]);
		return;
	}

	MarkSelection_SetStatic(command->chatCommand->Execute, &args[1], argsCount - 1);
}

static void ShowCouldNotFindCommand(const cc_string* command) {
	char buffer[64];
	cc_string message = String_FromArray(buffer);
	String_Format1(&message, "&b/%s&f is not a registered SPC command.", command);
	Chat_Add(&message);
}

static void ShowCannotStatic(const cc_string* command) {
	char buffer[64];
	cc_string message = String_FromArray(buffer);
	String_Format1(&message, "&b/%s&f does not support static mode.", command);
	Chat_Add(&message);
}

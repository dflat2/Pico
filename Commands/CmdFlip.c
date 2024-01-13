#include <stdio.h>

#include "CC_API/Chat.h"

#include "Axis.h"
#include "BlocksBuffer.h"
#include "MarkSelection.h"
#include "ParsingUtils.h"
#include "Messaging.h"
#include "SPCCommand.h"

static void ShowUsage();
static void Flip_Command(const cc_string* args, int argsCount);

static struct ChatCommand FlipCommand = {
	"Flip",
	Flip_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Flip X/Y/Z &f- Flips the copied cuboid around the given axis.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};

SPCCommand FlipSPCCommand = {
	.chatCommand = &FlipCommand,
	.canStatic = false
};

static void ShowUsage() {
	Message_Player("Usage: &b/Flip X&f, &b/Flip Y &for &b/Flip Z&f.");
}

static void Flip_Command(const cc_string* args, int argsCount) {
	if (BlocksBuffer_IsEmpty()) {
		Message_Player("There is nothing to flip. Do &b/Copy &ffirst.");
		return;
	}

	if (argsCount == 0) {
		ShowUsage();
		return;
	}
	
	Axis axis;

	if (!Parse_TryParseAxis(&args[0], &axis)) {
		return;
	}

	if (!BlocksBuffer_TryFlip(axis)) {
		Message_Player("Memory error");
		return;
	}

	switch (axis) {
		case AXIS_X:
			Message_Player("Switched around the &bX&f axis.");
			break;
		case AXIS_Y:
			Message_Player("Switched around the &bY&f axis.");
			break;
		default:
			Message_Player("Switched around the &bZ&f axis.");
			break;
	}
}

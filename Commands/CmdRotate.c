#include <stdio.h>

#include "ClassiCube/src/Chat.h"

#include "Axis.h"
#include "BlocksBuffer.h"
#include "MarkSelection.h"
#include "ParsingUtils.h"
#include "Messaging.h"
#include "SPCCommand.h"

static void ShowUsage();
static void Rotate_Command(const cc_string* args, int argsCount);

static struct ChatCommand RotateCommand = {
	"Rotate",
	Rotate_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Rotate [axis] [degrees] &f- Rotates the copied cuboid.",
		"&b[axis] &fmust be &bX&f, &bY&f (default) or &bZ&f.",
		"&b[degrees] &fmust be a multiple of 90.",
		NULL,
		NULL
	},
	NULL
};

SPCCommand RotateSPCCommand = {
	.chatCommand = &RotateCommand,
	.canStatic = false
};

static void ShowUsage() {
	Message_Player("Usage: &b/Rotate <axis> [degrees]&f.");
	Message_Player("&b[degrees]&f must be a multiple of 90.");
}

static void ShowRotated(Axis axis, int degrees) {
	char charAxis = 'X';

	if (axis == AXIS_Y) {
		charAxis = 'Y';
	} else if (axis == AXIS_Z) {
		charAxis = 'Z';
	}

	char buffer[64];
	cc_string message = String_FromArray(buffer);
	String_Format2(&message, "&fRotated around the %r axis by %i degrees.", &charAxis, &degrees);
	Chat_Add(&message);
}

static void Rotate_Command(const cc_string* args, int argsCount) {
	if (BlocksBuffer_IsEmpty()) {
		Message_Player("There is nothing to rotate. Do &b/Copy &ffirst.");
		return;
	}

	if (argsCount >= 3) {
		ShowUsage();
		return;
	}

	bool hasAxis = argsCount >= 1;
	bool hasDegrees = argsCount == 2;
	
	Axis axis;
	int degrees;

	if (!hasAxis) {
		axis = AXIS_Y;
	} else if (!Parse_TryParseAxis(&args[0], &axis)) {
		return;
	}

	if (!hasDegrees) {
		degrees = 90;
	} else if (!Parse_TryParseDegrees(&args[1], &degrees)) {
		return;
	}

	if (!BlocksBuffer_TryRotate(axis, degrees / 90)) {
		Message_Player("Memory error");
		return;
	}

	ShowRotated(axis, degrees);
}

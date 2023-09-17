#include <math.h>
#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "ParsingUtils.h"
#include "Array.h"
#include "SPCCommand.h"

typedef enum LineMode_ {
	MODE_NORMAL,
	MODE_WALL,
	MODE_BEZIER
} LineMode;

typedef struct LineArguments_ {
	LineMode mode;
	Brush* brush;
} LineArguments;

static void Line_Command(const cc_string* args, int argsCount);
static bool TryParseArguments(const cc_string* args, int argsCount, LineArguments* out_arguments);
static void ShowUsage();
static void CleanResources(void* args);
static void LineSelectionHandler(IVec3* marks, int count, void* object);
static void DoLine(IVec3 from, IVec3 to, Brush* brush);
static void DoWall(IVec3 from, IVec3 to, Brush* brush);
static void DoBezier(IVec3 from, IVec3 direction, IVec3 to, Brush* brush);
static int GreatestInteger2(int a, int b);
static int GreatestInteger3(int a, int b, int c);
static FVec3 Bezier(FVec3 from, FVec3 controlPoint, FVec3 to, float t);
static void Line(IVec3 from, IVec3 to, Brush* brush);

static struct ChatCommand LineCommand = {
	"Line",
	Line_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Line [mode] [brush/block]",
        "&fDraws a line between two points.",
		"&fList of modes: &bnormal&f (default), &bwall&f, &bbezier&f.",
		NULL,
		NULL
	},
	NULL
};

SPCCommand LineSPCCommand = {
	.chatCommand = &LineCommand,
	.canStatic = true
};

static int GreatestInteger2(int a, int b) {
	if (a > b) {
		return a;
	}

	return b;
}

static int GreatestInteger3(int a, int b, int c) {
	return GreatestInteger2(a, GreatestInteger2(b, c));
}

static void Line(IVec3 from, IVec3 to, Brush* brush) {
	int deltaX = to.X - from.X;
	int deltaY = to.Y - from.Y;
	int deltaZ = to.Z - from.Z;

	int steps = GreatestInteger3(abs(deltaX), abs(deltaY), abs(deltaZ));

	if (steps == 0) {
		return;
	}

	float incrementX = (float)deltaX / (float)steps;
	float incrementY = (float)deltaY / (float)steps;
	float incrementZ = (float)deltaZ / (float)steps;

	float x = from.X;
	float y = from.Y;
	float z = from.Z;

	for (int _ = 0; _ < steps; _++) {
		Draw_Brush(round(x), round(y), round(z), brush);
		x += incrementX;
		y += incrementY;
		z += incrementZ;
	}

	Draw_Brush(to.X, to.Y, to.Z, brush);
}

static void DoLine(IVec3 from, IVec3 to, Brush* brush) {
	Draw_Start("Line normal");
	Line(from, to, brush);
	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void DoWall(IVec3 from, IVec3 to, Brush* brush) {
	Draw_Start("Line wall");

	int deltaX = to.X - from.X;
	int deltaZ = to.Z - from.Z;

	int steps = GreatestInteger2(abs(deltaX), abs(deltaZ));

	if (steps == 0) {
		printf("deltaX = %d, deltaZ = %d\n", deltaX, deltaZ);
		Draw_End();
		Message_BlocksAffected(0);
		return;
	}

	float incrementX = (float)deltaX / (float)steps;
	float incrementZ = (float)deltaZ / (float)steps;

	float x = from.X;
	float z = from.Z;

	int yMin = from.Y;
	int yMax = to.Y;

	if (from.Y > to.Y) {
		yMin = to.Y;
		yMax = from.Y;
	}

	for (int _ = 0; _ < steps; _++) {
		for (int y = yMin; y <= yMax; y++) {
			Draw_Brush(round(x), round(y), round(z), brush);
		}

		x += incrementX;
		z += incrementZ;
	}

	for (int y = yMin; y <= yMax; y++) {
		Draw_Brush(to.X, round(y), to.Z, brush);
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static FVec3 Bezier(FVec3 from, FVec3 controlPoint, FVec3 to, float t) {
	// linear1 = (1 - t) * from + (t) * controlPoint
	FVec3 linear1 = FVec3_Add(FVec3_ScalarMultiply(from, 1 - t), FVec3_ScalarMultiply(controlPoint, t));

	// linear2 = (1 - t) * controlPoint + (t) * to
	FVec3 linear2 = FVec3_Add(FVec3_ScalarMultiply(controlPoint, 1 - t), FVec3_ScalarMultiply(to, t));

	// result = (1 - t) * linear1 + (t) * linear2
	FVec3 result = FVec3_Add(FVec3_ScalarMultiply(linear1, 1 - t), FVec3_ScalarMultiply(linear2, t));

	return result;
}

static void DoBezier(IVec3 from, IVec3 controlPoint, IVec3 to, Brush* brush) {
	Draw_Start("Line bezier");
	const int subDivisions = 64;

	FVec3 floatFrom = IVec3_ConvertFVec3(from);
	FVec3 floatControlPoint = IVec3_ConvertFVec3(controlPoint);
	FVec3 floatTo = IVec3_ConvertFVec3(to);

	IVec3 lineStart = from;
	IVec3 lineEnd;

	for (int i = 1; i <= subDivisions; i++) {
		lineEnd = FVec3_ConvertIVec3(
			Bezier(floatFrom, floatControlPoint, floatTo, i / (float)subDivisions)
		);

		Line(lineStart, lineEnd, brush);
		lineStart = lineEnd;
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void LineSelectionHandler(IVec3* marks, int count, void* object) {
    LineArguments* arguments = (LineArguments*)object;

	if (arguments->mode == MODE_NORMAL) {
		DoLine(marks[0], marks[1], arguments->brush);
	} else if (arguments->mode == MODE_WALL) {
		DoWall(marks[0], marks[1], arguments->brush);
	} else if (arguments->mode == MODE_BEZIER) {
		DoBezier(marks[0], marks[1], marks[2], arguments->brush);
	}
}

static void CleanResources(void* object) {
	LineArguments* arguments = (LineArguments*) object;
	Brush_Free(arguments->brush);
	free(arguments);
}

static void ShowUsage() {
	Message_Player("Usage: &b/Line normal/wall/bezier [brush/block]&f.");
}

static bool TryParseArguments(const cc_string* args, int argsCount, LineArguments* out_arguments) {
    cc_string modesString[] = {
        String_FromConst("normal"),
        String_FromConst("wall"),
        String_FromConst("bezier"),
    };

	size_t modesCount = sizeof(modesString) / sizeof(modesString[0]);

	bool hasBlockOrBrush = (argsCount >= 2) ||
		((argsCount == 1) && (!Array_ContainsString(&args[0], modesString, modesCount)));
	bool hasMode = (argsCount >= 1) && Array_ContainsString(&args[0], modesString, modesCount);

	if (hasMode) {
		out_arguments->mode = Array_IndexOfStringCaseless(&args[0], modesString, modesCount);
	} else {
		out_arguments->mode = MODE_NORMAL;
	}

	if (hasMode && out_arguments->mode == -1) {
		Message_ShowUnknownMode(&args[0]);
		Message_ShowAvailableModes(modesString, modesCount);
		return false;
	}

	Brush* brush = Brush_CreateEmpty();

	if (hasBlockOrBrush) {
		int brushIndex;

		if (hasMode) {
			brushIndex = 1;
		} else {
			brushIndex = 0;
		}

		// Checks that there are no trailing blocks in the command.
		bool isBlock = args[brushIndex].buffer[0] != '@';
		if (isBlock && argsCount > (brushIndex + 1)) {
			ShowUsage();
			return false;
		}

		if (!Parse_TryParseBlockOrBrush(&args[brushIndex], argsCount - brushIndex, brush)) {
			return false;
		}

		out_arguments->brush = brush;
		return true;
	} else {
		if (!Brush_TryCreateNormal(BLOCK_AIR, true, brush)) {
			return false;
		}
		out_arguments->brush = brush;
		return true;
	}
}

static void Line_Command(const cc_string* args, int argsCount) {
	LineArguments* arguments = allocate(1, sizeof(LineArguments));
	
	if (!TryParseArguments(args, argsCount, arguments)) {
		free(arguments);
		MarkSelection_Abort();
		return;
	}

	if (arguments->mode != MODE_BEZIER) {
		MarkSelection_Make(LineSelectionHandler, 2, arguments, CleanResources);
		Message_Player("&fPlace or break two blocks to determine the endpoints.");
	} else {
		MarkSelection_Make(LineSelectionHandler, 3, arguments, CleanResources);
		Message_Player("Place or break two blocks to determine the endpoints, then another for the direction.");
	}
}

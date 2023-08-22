#include "CC_API/Chat.h"
#include "CC_API/Inventory.h"
#include "CC_API/World.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "MemoryAllocation.h"
#include "Messaging.h"
#include "Vectors.h"
#include "ParsingUtils.h"
#include "Array.h"
#include "SPCCommand.h"
#include "WorldUtils.h"

typedef struct ReplaceArguments_ {
	BlockID block;
	Brush* brush;
	bool replaceNot;
} ReplaceArguments;

static void ReplaceNot_Command(const cc_string* args, int argsCount);
static void Replace_Command(const cc_string* args, int argsCount);
static void ReplaceAll_Command(const cc_string* args, int argsCount);
static bool TryParseArguments(const cc_string* args, int argsCount, ReplaceArguments* out_arguments);
static void ShowUsage();
static void CleanResources(void* args);
static void ReplaceSelectionHandler(IVec3* marks, int count, void* object);
static void DoReplace(IVec3 min, IVec3 max, BlockID block, Brush* brush, bool replaceNot);

static struct ChatCommand ReplaceCommand = {
	"Replace",
	Replace_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Replace <oldblock> [newblock]",
		"Replaces &b<oldblock> &fwith &b[newblock]&f between two points.",
		"If &b[newblock] &fis not given, replaces with your held block.",
		"&b/Replace <oldblock> <brush>",
		"Replaces &b<oldblock> &fwith the output of &b<brush>&f."
	},
	NULL
};

SPCCommand ReplaceSPCCommand = {
	.chatCommand = &ReplaceCommand,
	.canStatic = true
};

static struct ChatCommand ReplaceAllCommand = {
	"ReplaceAll",
	ReplaceAll_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/ReplaceAll <oldblock> [block/brush]",
		"Shortcut for the &b/Replace | /MarkAll &fcombination.",
		NULL,
		NULL,
		NULL,
	},
	NULL
};

SPCCommand ReplaceAllSPCCommand = {
	.chatCommand = &ReplaceAllCommand,
	.canStatic = false
};

static struct ChatCommand ReplaceNotCommand = {
	"ReplaceNot",
	ReplaceNot_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/ReplaceNot <block>> [newblock]",
		"Replaces all blocks but &b<block>&fwith &b[newblock]&f.",
		"If &b[newblock] &fis not given, replaces with your held block.",
		"&b/ReplaceNot <block> <brush>",
		"Does the same, but replaces with the output of &b<brush>&f."
	},
	NULL
};

SPCCommand ReplaceNotSPCCommand = {
	.chatCommand = &ReplaceNotCommand,
	.canStatic = true
};

static void DoReplace(IVec3 min, IVec3 max, BlockID block, Brush* brush, bool replaceNot) {
	Draw_Start("Replace");
	BlockID current;

	for (int x = min.X; x <= max.X; x++) {
		for (int y = min.Y; y <= max.Y; y++) {
			for (int z = min.Z; z <= max.Z; z++) {
				current = GetBlock(x, y, z);

				if ((!replaceNot && current == block) || (replaceNot && current != block)) {
					Draw_Brush(x, y, z, brush);
				}
			}
		}
	}

	int blocksAffected = Draw_End();
	Message_BlocksAffected(blocksAffected);
}

static void ReplaceSelectionHandler(IVec3* marks, int count, void* object) {
    if (count != 2) {
        return;
    }

    ReplaceArguments* arguments = (ReplaceArguments*)object;
    DoReplace(Min(marks[0], marks[1]), Max(marks[0], marks[1]),
			arguments->block, arguments->brush, arguments->replaceNot);
}

static void CleanResources(void* args) {
	ReplaceArguments* arguments = (ReplaceArguments*) args;
	Brush_Free(arguments->brush);
	free(arguments);
}

static void ShowUsage() {
	Message_Player("Usage: &b/Replace[Not/All] <block> [brush/block]&f.");
}

static bool TryParseArguments(const cc_string* args, int argsCount, ReplaceArguments* out_arguments) {
	if (argsCount == 0) {
		ShowUsage();
		return false;
	}

	if (!TryParseBlock(&args[0], &out_arguments->block)) {
		return false;
	}

	bool hasBlockOrBrush = (argsCount >= 2);
	Brush* brush = Brush_CreateEmpty();

	if (hasBlockOrBrush) {
		bool isBlock = args[1].buffer[0] != '@';
		if (isBlock && argsCount > 2) {
			ShowUsage();
			return false;
		}

		if (!Parse_TryParseBlockOrBrush(&args[1], argsCount - 1, brush)) {
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

static void Replace_Command(const cc_string* args, int argsCount) {
	ReplaceArguments* arguments = allocate(1, sizeof(ReplaceArguments));
	arguments->replaceNot = false;
	
	if (!TryParseArguments(args, argsCount, arguments)) {
		free(arguments);
		MarkSelection_Abort();
		return;
	}

    MarkSelection_Make(ReplaceSelectionHandler, 2, arguments, CleanResources);
    Message_Player("Place or break two blocks to determine the edges.");
}

static void ReplaceAll_Command(const cc_string* args, int argsCount) {
	ReplaceArguments* arguments = allocate(1, sizeof(ReplaceArguments));
	arguments->replaceNot = false;
	
	if (!TryParseArguments(args, argsCount, arguments)) {
		free(arguments);
		MarkSelection_Abort();
		return;
	}

    MarkSelection_Make(ReplaceSelectionHandler, 2, arguments, CleanResources);

    IVec3 low = { .X = 0, .Y = 0, .Z = 0 };
    IVec3 high = { .X = World.Width - 1, .Y = World.Height - 1, .Z = World.Length - 1 };

    MarkSelection_DoMark(low);
    MarkSelection_DoMark(high);
}

static void ReplaceNot_Command(const cc_string* args, int argsCount) {
	ReplaceArguments* arguments = allocate(1, sizeof(ReplaceArguments));
	arguments->replaceNot = true;
	
	if (!TryParseArguments(args, argsCount, arguments)) {
		free(arguments);
		MarkSelection_Abort();
		return;
	}

    MarkSelection_Make(ReplaceSelectionHandler, 2, arguments, CleanResources);
    Message_Player("Place or break two blocks to determine the edges.");
}

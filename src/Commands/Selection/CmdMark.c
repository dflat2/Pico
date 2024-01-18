#include "ClassiCube/src/Chat.h"

#include "MarkSelection.h"
#include "WorldUtils.h"
#include "Messaging.h"

static void Mark_Command(const cc_string* args, int argsCount);

struct ChatCommand MarkCommand = {
	"Mark",
	Mark_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Mark [x y z]",
		"Places a marker for selections, e.g. &b/Z&f.",
		"You may use parenthesis to indicate relative position.",
		"For example, &b/Mark (0) (2) (0) &fwill mark above you.",
		NULL
	},
	NULL
};

static void Mark_Command(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        MarkSelection_DoMark(SnapToWorldBoundaries(GetCurrentPlayerPosition()));
        return;
    }

    if (argsCount != 3) {
        Message_CommandUsage(MarkCommand);
        return;
    }

    IVec3 target;
    
    if (!TryParseCoordinates(args, &target)) {
        return;
    }
    
    MarkSelection_DoMark(SnapToWorldBoundaries(target));
}

#include "CC_API/Chat.h"

#include "MarkSelection.h"
#include "WorldUtils.h"
#include "Messaging.h"

static void Mark_Command(const cc_string* args, int argsCount) {
    if (argsCount == 0) {
        DoMark(SnapToWorldBoundaries(GetCurrentPlayerPosition()));
        return;
    }

    if (argsCount != 3) {
        PlayerMessage("&fUsage: &b/Mark <x> <y> <z> &for &b/Mark&f.");
        return;
    }

    IVec3 target;
    
    if (!TryParseCoordinates(args, &target)) {
        return;
    }
    
    DoMark(SnapToWorldBoundaries(target));
}

struct ChatCommand MarkCommand = {
	"Mark",
	Mark_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Mark <x> <y> <z>",
		"&fPlaces a marker for selections, e.g. &b/Z&f.",
		"&fYou may use parenthesis to indicate relative position.",
		"&fFor example, &b/Mark (0) (2) (0) &fwill mark above you.",
		"&fMarks outside of the world are snapped on one of its sides."
	},
	NULL
};
#include "CC_API/Chat.h"
#include "CC_API/World.h"

#include "Messaging.h"
#include "MarkSelection.h"

static void MarkAll_Command(const cc_string* args, int argsCount) {
	if (argsCount != 0) {
		PlayerMessage("&fUsage: &b/MarkAll&f.");
		return;
	}

	if (RemainingMarks() == 0) {
		PlayerMessage("&fCannot mark, no selection in progress.");
		return;
	}
	else if (RemainingMarks() == 1) {
		PlayerMessage("&fCannot &b/MarkAll &funless there are at least two remaining marks.");
		return;
	}

    IVec3 low = { .X = 0, .Y = 0, .Z = 0 };
    IVec3 high = { .X = World.Width - 1, .Y = World.Height - 1, .Z = World.Length - 1 };

    DoMark(low);
    DoMark(high);
}

struct ChatCommand MarkAllCommand = {
	"MarkAll",
	MarkAll_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/MarkAll",
		"&fPlaces markers in min and max corners of the map.",
		NULL,
        NULL,
        NULL
	},
	NULL
};

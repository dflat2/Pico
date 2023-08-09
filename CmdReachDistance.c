#include "CC_API/Chat.h"
#include "CC_API/Entity.h"

#include "Messaging.h"

#define DEFAULT_REACH_DISTANCE 5.0f

static void SetReachDistance(struct LocalPlayer* player, float reachDistance) {
	if (reachDistance < 0.0f) {
		return;
	}

	player->ReachDistance = reachDistance;
}

static void ResetReachDistance(struct LocalPlayer* player) {
	SetReachDistance(player, DEFAULT_REACH_DISTANCE);
}

static void ReachDistance_Command(const cc_string* args, int argsCount) {
	struct LocalPlayer* player = (struct LocalPlayer*)Entities.List[ENTITIES_SELF_ID];

	if (argsCount == 0) {
		ResetReachDistance(player);
		PlayerMessage("&fYour reach distance has been reset to &b5.0&f.");
		return;
	}

	if (argsCount >= 2) {
		PlayerMessage("&fUsage: &b/ReachDistance [distance]");
		return;
	}

	float distance;
	bool success = Convert_ParseFloat(&args[0], &distance);

	if (!success)
	{
		PlayerMessage("&fUsage: &b/ReachDistance [distance]&f.");
		return;
	}

	if (distance < 0.0f)
	{
		PlayerMessage("&fDistance must be positive or 0.");
		return;
	}

	SetReachDistance(player, distance);

	char msgBuffer[512];
	cc_string message = String_FromArray(msgBuffer);
	String_Format1(&message, "&fYour reach distance has been set to &b%f2&f.", &distance);
	Chat_Add(&message);
}

struct ChatCommand ReachDistanceCommand = {
	"ReachDistance",
	ReachDistance_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/ReachDistance [distance]",
		"&fSets your reach distance to &bdistance&f.",
		"&fResets to default (&b5.0&f) if no &bdistance &fis given.",
		NULL,
		NULL
	},
	NULL
};

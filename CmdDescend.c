#include "CC_API/Chat.h"
#include "CC_API/Entity.h"

#include "Messaging.h"
#include "WorldUtils.h"

static bool TryFindBelow(Vec3 currentPosition, Vec3* descendPosition) {
	int x = (int) currentPosition.X;
	int y = (int) currentPosition.Y;
	int z = (int) currentPosition.Z;

	descendPosition->X = currentPosition.X;
	descendPosition->Z = currentPosition.Z;

	for (int yCandidate = y - 1; yCandidate >= 0; yCandidate--) {
		if (CanStandOnBlock(x, yCandidate, z)) {
			descendPosition->Y = (float)yCandidate;
			return true;
		}
	}

	return false;
}

static void Descend_Command(const cc_string* args, int argsCount) {
	if (argsCount >= 1) {
		PlayerMessage("&fUsage: &b/Descend");
		return;
	}

	struct LocalPlayer* player = (struct LocalPlayer*)Entities.List[ENTITIES_SELF_ID];
	struct Entity playerEntity = player->Base;

	Vec3 currentPosition = playerEntity.Position;
	Vec3 descendPosition;

	bool success = TryFindBelow(currentPosition, &descendPosition);

	if (!success) {
		PlayerMessage("&fThere are no blocks below to descend to.");
		return;
	}

	struct LocationUpdate update;
	update.flags = LU_HAS_POS;
	update.pos = descendPosition;

	playerEntity.VTABLE->SetLocation(&playerEntity, &update);
	PlayerMessage("&fTeleported you down.");
}

struct ChatCommand DescendCommand = {
	"Descend",
	Descend_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Descend",
		"&fTeleports you to the first free space below you.",
		NULL,
		NULL,
		NULL
	},
	NULL
};

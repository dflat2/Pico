#include "CC_API/Chat.h"
#include "CC_API/Entity.h"
#include "CC_API/World.h"

#include "Messaging.h"
#include "WorldUtils.h"

static bool TryFindAbove(Vec3 currentPosition, Vec3* ascendPosition)
{
	int x = (int) currentPosition.X;
	int y = (int) currentPosition.Y;
	int z = (int) currentPosition.Z;

	ascendPosition->X = currentPosition.X;
	ascendPosition->Z = currentPosition.Z;

	for (int yCandidate = y + 1; yCandidate <= World.Height; yCandidate++)
	{
		if (CanStandOnBlock(x, yCandidate, z))
		{
			ascendPosition->Y = (float)yCandidate;
			return true;
		}
	}

	return false;
}

static void Ascend_Command(const cc_string* args, int argsCount)
{
	if (argsCount >= 1)
	{
		PlayerMessage("&fUsage: &b/Ascend");
		return;
	}

	struct LocalPlayer* player = (struct LocalPlayer*)Entities.List[ENTITIES_SELF_ID];
	struct Entity playerEntity = player->Base;

	Vec3 currentPosition = playerEntity.Position;
	Vec3 ascendPosition;

	bool success = TryFindAbove(currentPosition, &ascendPosition);

	if (!success)
	{
		PlayerMessage("&fThere are no blocks above to ascend to.");
		return;
	}

	struct LocationUpdate update;
	update.flags = LU_HAS_POS;
	update.pos = ascendPosition;

	playerEntity.VTABLE->SetLocation(&playerEntity, &update);
	PlayerMessage("&fTeleported you up.");
}

struct ChatCommand AscendCommand = {
	"Ascend",
	Ascend_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY,
	{
		"&b/Ascend",
		"&fTeleports you to the first free space above you.",
		NULL,
		NULL,
		NULL
	},
	NULL
};



#include "CC_API/BlockID.h"
#include "CC_API/Block.h"
#include "CC_API/Chat.h"
#include "CC_API/Core.h"
#include "CC_API/Entity.h"
#include "CC_API/Vectors.h"
#include "CC_API/World.h"

#include "Messaging.h"
#include "WorldUtils.h"

static void Brushes_Command(const cc_string* args, int argsCount) {
	PlayerMessage("Available brushes:");
	PlayerMessage(" &b@Checkered <block1> <block2>");
	PlayerMessage(" &b@Rainbow");
	PlayerMessage(" &b@Solid <block>");
}

struct ChatCommand BrushesCommand = {
	"Brushes",
	Brushes_Command,
	COMMAND_FLAG_SINGLEPLAYER_ONLY, {
		"&b/Brushes&f - Lists available brushes.",
		NULL,
		NULL,
		NULL,
		NULL
	},
	NULL
};


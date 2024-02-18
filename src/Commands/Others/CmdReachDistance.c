#include <stdbool.h>

#include "ClassiCube/src/Entity.h"
#include "ClassiCube/src/Constants.h"

#include "Message.h"

static void ReachDistance_Command(const cc_string* args, int argsCount);

struct ChatCommand ReachDistanceCommand = {
    "ReachDistance",
    ReachDistance_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/ReachDistance [distance]",
        "Sets your reach distance to &bdistance&f.",
        "\x07 &bdistance&f: positive non-zero decimal (default &b5.0&f).",
        NULL,
        NULL
    },
    NULL
};

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
        Message_Player("Your reach distance has been reset to &b5.0&f.");
        return;
    }

    if (argsCount >= 2) {
        Message_CommandUsage(ReachDistanceCommand);
        return;
    }

    float distance;
    bool success = Convert_ParseFloat(&args[0], &distance);

    if (!success) {
        Message_CommandUsage(ReachDistanceCommand);
        return;
    }

    if (distance < 0.0f) {
        Message_Player("&bdistance must be a positive decimal number.");
        return;
    }

    SetReachDistance(player, distance);

    char msgBuffer[STRING_SIZE];
    cc_string message = String_FromArray(msgBuffer);
    String_Format1(&message, "Your reach distance has been set to &b%f2&f.", &distance);
    Chat_Add(&message);
}

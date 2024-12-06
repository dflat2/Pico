#include <stdbool.h>

#include "ClassiCube/src/Entity.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Chat.h"

#include "Message.h"

static void Speed_Command(const cc_string* args, int argsCount);

struct ChatCommand SpeedCommand = {
    "Speed",
    Speed_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Speed [speed]",
        "Sets your base horizontal speed to &bspeed&f.",
        "\x07 &bspeed&f: positive decimal (default &b1.0&f).",
        NULL,
        NULL
    },
    NULL
};

static void SetSpeed(struct LocalPlayer* player, float speed) {
    if (speed < 0.0f) {
        return;
    }

    player->Hacks.BaseHorSpeed = speed;
}

static void ResetSpeed(struct LocalPlayer* player) {
    SetSpeed(player, 1.0f);
}

static void Speed_Command(const cc_string* args, int argsCount) {
    struct LocalPlayer* player = Entities.CurPlayer;

    if (argsCount == 0) {
        ResetSpeed(player);
        Message_Player("Your speed has been reset to &b1.0&f.");
        return;
    }

    if (argsCount >= 2) {
        Message_CommandUsage(SpeedCommand);
        return;
    }

    float speed;
    bool success = Convert_ParseFloat(&args[0], &speed);

    if (!success) {
        Message_CommandUsage(SpeedCommand);
        return;
    }

    if (speed < 0.0f) {
        Message_Player("&bspeed must be a positive decimal number.");
        return;
    }

    SetSpeed(player, speed);

    char msgBuffer[STRING_SIZE];
    cc_string message = String_FromArray(msgBuffer);
    String_Format1(&message, "Your speed has been set to &b%f2&f.", &speed);
    Chat_Add(&message);
}

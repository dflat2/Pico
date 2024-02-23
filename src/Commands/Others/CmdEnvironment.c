#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/Constants.h"

#include "Message.h"

static void DescribeEnvironment(void) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    Message_Player("Edge and sides:");
    cc_string UNSAFE_edgeBlock = Block_UNSAFE_GetName(Env.EdgeBlock);
    cc_string UNSAFE_sidesBlock = Block_UNSAFE_GetName(Env.SidesBlock);
    int sidesHeight = Env.EdgeHeight + Env.SidesOffset;

    String_Format2(&message, "\x07 &bEdgeBlock&f: &b%s &f\x07 &bSidesBlock&f: &b%s&f.", &UNSAFE_edgeBlock, &UNSAFE_sidesBlock);
    Chat_Add(&message);
    message.length = 0;
    String_Format2(&message, "\x07 &bSidesHeight&f: &b%i &f\x07 &bSidesOffset&f: &b%i&f.", &sidesHeight, &Env.SidesOffset);
    Chat_Add(&message);
    message.length = 0;

    Message_Player("Clouds:");
    int cloudsHeight = Env.CloudsHeight;
    float cloudsSpeed = Env.CloudsSpeed;

    String_Format2(&message, "\x07 &bCloudsHeight&f: &b%i &f\x07 &bCloudsSpeed&f: &b%f&f.", &cloudsHeight, &cloudsSpeed);
    Chat_Add(&message);
    message.length = 0;

    Message_Player("Weather:");
    float weatherSpeed = Env.WeatherSpeed;
    float weatherFade = Env.WeatherFade;

    String_Format2(&message, "\x07 &bWeatherSpeed&f: &b%f &f\x07 &bWeatherFade&f: &b%f&f.", &weatherSpeed, &weatherFade);
    Chat_Add(&message);
    message.length = 0;

    // TODO
    // Message_Player("\x07 &bWeather&f: ");
    // Message_Player("\x07 &bSkyColour&f: ");
    // Message_Player("\x07 &bFogColour&f: ");
    // Message_Player("\x07 &bCloudsColour&f: ");
    // Message_Player("\x07 &bSkyboxColour&f: ");
    // Message_Player("\x07 &bSunColour&f: ");
    // Message_Player("\x07 &bShadowColour&f: ");
}

static void Environment_Command(const cc_string* args, int argsCount);

struct ChatCommand EnvironmentCommand = {
    "Environment",
    Environment_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Environment",
        "Lists all environment options and their current value.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Environment_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(EnvironmentCommand);
        return;
    }

    DescribeEnvironment();

    // if (argsCount >= 3) {
    //     Message_CommandUsage(EnvironmentCommand);
    //     return;
    // }

    // if (argsCount == 0) {
    //     DescribeEnvironment();
    //     return;
    // } else {
    //     Message_Player("Setting values is not yet implemented.");
    // }
}

#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/Constants.h"

#include "Message.h"
#include "Format.h"

static void DescribeEnvironment(void) {
    Message_Player("~ &bEnvironment &f~");

    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);
    char colour1Buffer[STRING_SIZE];
    cc_string colour1 = String_FromArray(colour1Buffer);
    char colour2Buffer[STRING_SIZE];
    cc_string colour2 = String_FromArray(colour2Buffer);

    Message_Player("Edge and sides:");
    cc_string UNSAFE_edgeBlock = Block_UNSAFE_GetName(Env.EdgeBlock);
    cc_string UNSAFE_sidesBlock = Block_UNSAFE_GetName(Env.SidesBlock);

    String_AppendConst(&message, "\x07 &bEdgeBlock&f: &b");
    String_AppendString(&message, &UNSAFE_edgeBlock);
    String_AppendConst(&message, " &f\x07 &bSidesBlock&f: &b");
    String_AppendString(&message, &UNSAFE_sidesBlock);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
    message.length = 0;

    String_AppendConst(&message, "\x07 &bSidesHeight&f: &b");
    String_AppendInt(&message, Env.EdgeHeight + Env.SidesOffset);
    String_AppendConst(&message, " &f\x07 &bSidesOffset&f: &b");
    String_AppendInt(&message, Env.SidesOffset);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
    message.length = 0;

    Message_Player("Clouds:");
    String_AppendConst(&message, "\x07 &bCloudsHeight&f: &b");
    String_AppendInt(&message, Env.CloudsHeight);
    String_AppendConst(&message, " &f\x07 &bCloudsSpeed&f: &b");
    String_AppendFloat(&message, Env.CloudsSpeed, 2);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
    message.length = 0;

    Message_Player("Weather:");
    String_AppendConst(&message, "\x07 &bWeatherSpeed&f: &b");
    String_AppendFloat(&message, Env.WeatherSpeed, 2);
    String_AppendConst(&message, " &f\x07 &bWeatherFade&f: &b");
    String_AppendFloat(&message, Env.WeatherFade, 2);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
    message.length = 0;

    if (Env.Weather == WEATHER_SUNNY) {
        Message_Player("\x07 &bWeather&f: &bSunny&f.");
    } else if (Env.Weather == WEATHER_RAINY) {
        Message_Player("\x07 &bWeather&f: &bRainy&f.");
    } else {
        Message_Player("\x07 &bWeather&f: &bSnowy&f.");
    }

    Message_Player("Colours:");
    Format_PackedColor(&colour1, Env.SkyCol);
    Format_PackedColor(&colour2, Env.FogCol);
    String_Format2(&message, "\x07 &bSkyColour&f: &b%s &f\x07 &bFogColour&f: &b%s&f.", &colour1, &colour2);
    Chat_Add(&message);
    message.length = 0;
    colour1.length = 0;
    colour2.length = 0;
    Format_PackedColor(&colour1, Env.CloudsCol);
    Format_PackedColor(&colour2, Env.SkyboxCol);
    String_Format2(&message, "\x07 &bCloudsColour&f: &b%s &f\x07 &bSkyboxColour&f: &b%s&f.", &colour1, &colour2);
    Chat_Add(&message);
    message.length = 0;
    colour1.length = 0;
    colour2.length = 0;
    Format_PackedColor(&colour1, Env.SunCol);
    Format_PackedColor(&colour2, Env.ShadowCol);
    String_Format2(&message, "\x07 &bSunColour&f: &b%s &f\x07 &bShadowColour&f: &b%s&f.", &colour1, &colour2);
    Chat_Add(&message);
    message.length = 0;
    colour1.length = 0;
    colour2.length = 0;
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

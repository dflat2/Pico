#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Block.h"
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Chat.h"

#include "Parse.h"
#include "Message.h"
#include "Format.h"

static void SetEdgeBlock(const cc_string* argument) {
	BlockID block;

	if (Parse_TryParseBlock(argument, &block)) {
		Env_SetEdgeBlock(block);
	}
}

static void SetSidesBlock(const cc_string* argument) {
	BlockID block;

	if (Parse_TryParseBlock(argument, &block)) {
		Env_SetSidesBlock(block);
	}
}

static void SetEdgeHeight(const cc_string* argument) {
    int value;

    if (Parse_TryParseNumber(argument, &value)) {
        Env_SetEdgeHeight(value);
    }
}

static void SetSidesOffset(const cc_string* argument) {
    int value;

    if (Parse_TryParseNumber(argument, &value)) {
        Env_SetSidesOffset(value);
    }
}

static void SetCloudsHeight(const cc_string* argument) {
    int value;

    if (Parse_TryParseNumber(argument, &value)) {
        Env_SetCloudsHeight(value);
    }
}

static void SetCloudsSpeed(const cc_string* argument) {
    float value;

    if (Parse_TryParseFloat(argument, &value)) {
        Env_SetCloudsSpeed(value);
    }
}

static void SetWeatherSpeed(const cc_string* argument) {
    float value;

    if (Parse_TryParseFloat(argument, &value)) {
        Env_SetWeatherSpeed(value);
    }
}

static void SetWeatherFade(const cc_string* argument) {
    float value;

    if (Parse_TryParseFloat(argument, &value)) {
        Env_SetWeatherFade(value);
    }
}

static void SetWeather(const cc_string* argument) {
    enum Weather_ value;

    if (String_CaselessEqualsConst(argument, "Sunny")) {
        value = WEATHER_SUNNY;
    } else if (String_CaselessEqualsConst(argument, "Rainy")) {
        value = WEATHER_RAINY;
    } else if (String_CaselessEqualsConst(argument, "Snowy")) {
        value = WEATHER_SNOWY;
    } else {
        Message_Player("Usage: &b/Environment weather sunny/rainy/snowy&f.");
        return;
    }

    Env_SetWeather((int)value);
}

static void SetSkyCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetSkyCol(value);
    }
}

static void SetFogCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetFogCol(value);
    }
}

static void SetCloudsCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetCloudsCol(value);
    }
}

static void SetSkyboxCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetSkyboxCol(value);
    }
}

static void SetSunCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetSunCol(value);
    }
}

static void SetShadowCol(const cc_string* argument) {
    PackedCol value;

    if (Parse_TryParseColor(argument, &value)) {
        Env_SetShadowCol(value);
    }
}

static void ShowUnknownEnvironmentOption(const cc_string* option) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);
    String_Format1(&message, "Unknown environment option: &b%s&f.", option);
    Chat_Add(&message);
}

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

    String_AppendConst(&message, "\x07 &bEdgeHeight&f: &b");
    String_AppendInt(&message, Env.EdgeHeight);
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
        "&b/Environment <option> <value>",
        "Sets &boption &fto &bvalue&f.",
        NULL
    },
    NULL
};

static void Environment_Command(const cc_string* args, int argsCount) {
    if (argsCount >= 3) {
        Message_CommandUsage(EnvironmentCommand);
        return;
    }

    if (argsCount == 0) {
        DescribeEnvironment();
        return;
    }
    
    if (String_CaselessEqualsConst(&args[0], "EdgeBlock")) {
        SetEdgeBlock(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "SidesBlock")) {
        SetSidesBlock(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "EdgeHeight")) {
        SetEdgeHeight(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "SidesOffset")) {
        SetSidesOffset(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "CloudsHeight")) {
        SetCloudsHeight(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "CloudsSpeed")) {
        SetCloudsSpeed(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "WeatherSpeed")) {
        SetWeatherSpeed(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "WeatherFade")) {
        SetWeatherFade(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "Weather")) {
        SetWeather(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "SkyColour")) {
        SetSkyCol(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "FogColour")) {
        SetFogCol(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "CloudsColour")) {
        SetCloudsCol(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "SkyboxColour")) {
        SetSkyboxCol(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "SunColour")) {
        SetSunCol(&args[1]);
    } else if (String_CaselessEqualsConst(&args[0], "ShadowColour")) {
        SetShadowCol(&args[1]);
    } else {
        ShowUnknownEnvironmentOption(&args[0]);
    }
}

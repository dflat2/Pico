#include "ClassiCube/src/World.h"
#include "ClassiCube/src/Constants.h"

#include "Message.h"
#include "Format.h"

static void MapInfo_Command(const cc_string* args, int argsCount);

struct ChatCommand MapInfoCommand = {
    "MapInfo",
    MapInfo_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/MapInfo",
        "Displays various information about MapInfo.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void MapInfo_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(MapInfoCommand);
        return;
    }

    Message_Player("&b\x10 &fMap information:");

    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    // Map name
    if (World.Name.length >= 1) {
        String_Format1(&message, "\x07 Name: &b%s&f.", &World.Name);
        Chat_Add(&message);
        message.length = 0;
    } else {
        Message_Player("\x07 Name: &7unnamed&f.");
    }

    // Dimensions
    char widthBuffer[STRING_SIZE];
    cc_string width = String_FromArray(widthBuffer);
    Format_Int32(&width, World.Width);

    char heightBuffer[STRING_SIZE];
    cc_string height = String_FromArray(heightBuffer);
    Format_Int32(&height, World.Height);

    char lengthBuffer[STRING_SIZE];
    cc_string length = String_FromArray(lengthBuffer);
    Format_Int32(&length, World.Length);

    String_Format3(&message, "\x07 Dimensions: &b(%s, %s, %s)&f.", &width, &height, &length);
    Chat_Add(&message);
    message.length = 0;

    // Seed
    String_Format1(&message, "\x07 Seed: &b%i&f.", &World.Seed);
    Chat_Add(&message);
    message.length = 0;
}

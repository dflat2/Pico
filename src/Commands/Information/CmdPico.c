#include "Message.h"

static void Pico_Command(const cc_string* args, int argsCount);

struct ChatCommand PicoCommand = {
    "Pico",
    Pico_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Pico",
        "Displays various information about Pico.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Pico_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(PicoCommand);
        return;
    }

    Message_Player("&a-------------------------------------------------");
    Message_Player("~ &bPico &f~");
    Message_Player("A singleplayer commands plugin for ClassiCube.");
    Message_Player("\x07 Version: &b0.3.1");
    Message_Player("\x07 Made for ClassiCube version: &b1.3.7");
    Message_Player("\x07 Github: &bhttps://github.com/dflat2/Pico");
    Message_Player("\x07 Bug report: &bhttps://github.com/dflat2/Pico/issues");
    Message_Player("Made with &c\x03 &fby &bD_Flat&f.");
    Message_Player("&a------------------------------------------------");
    Message_Player("This plugin is still in progress and is not fully documented.");
    Message_Player("Although it should be pretty stable, it is recommended that you");
    Message_Player("save your progress regularly. Do &b/Help <command> &ffor");
    Message_Player("instructions about a command.");
    Message_Player("&a------------------------------------------------");
    Message_Player("&cWarning. &fPlease use:");
    Message_Player("\x07 &b/Z &finstead of &b/Cuboid&f");
    Message_Player("\x07 &b/R &finstead of &b/Replace&f");
    Message_Player("\x07 &b/P &finstead of &b/Place&f");
    Message_Player("As default build commands are incompatible with Pico.");
    Message_Player("&a------------------------------------------------");
}

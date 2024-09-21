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

    Message_Player("~ &bPico &f~");
    Message_Player("A singleplayer commands plugin for ClassiCube.");
    Message_Player("\x07 Version: &b0.3.0");
    Message_Player("\x07 Made for ClassiCube version: &b1.3.7");
    Message_Player("\x07 Github: &bhttps://github.com/dflat2/Pico");
    Message_Player("\x07 Bug report: &bhttps://github.com/dflat2/Pico/issues");
    Message_Player("\x07 Discord: &bd_flat");
    Message_Player("Made with &c\x03 &fby &bD_Flat&f.");
}

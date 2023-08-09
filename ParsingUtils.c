#include "CC_API/String.h"
#include "CC_API/Inventory.h"
#include "CC_API/Chat.h"
#include "CC_API/Block.h"

#include "Messaging.h"

#define SUCCESS 1
#define FAILURE 0

static void ShowUnknownMode(const cc_string* mode) {
    char message[64];
    cc_string cc_message = { message, .length = 0, .capacity = sizeof(message) };
    String_Format1(&cc_message, "&fUnkown mode: &b%s&f.", mode);
    Chat_Add(&cc_message);
}

static void ShowAvailableModes(const cc_string* modes, int modesCount) {
    char buffer[128];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_AppendConst(&message, "&fAvailable modes: &b");

    for (int i = 0; i < modesCount - 1; i++) {
        String_AppendString(&message, &modes[i]);
        String_AppendConst(&message, "&f, &b");
    }

    String_AppendString(&message, &modes[modesCount - 1]);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
}

static void ShowUnkownBlock(const cc_string* block) {
    char buffer[128];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_Format1(&message, "&fCould not find block specified: &b%s&f.", block);
    Chat_Add(&message);
}

static bool TryParseBlock(const cc_string* blockString, BlockID* block) {
    int i_block;
    i_block = Block_Parse(blockString);

    if (i_block == -1) {
        ShowUnkownBlock(blockString);
        return FAILURE;
    }

    *block = (BlockID)i_block;
    return SUCCESS;
}

static bool TryParseMode(const cc_string* modes, int modesCount, const cc_string* modeString, int* mode) {
    for (int i = 0; i < modesCount; i++) {
       if (String_CaselessEquals(&modes[i], modeString)) {
            *mode = i;
            return SUCCESS;
       }
    }

    ShowUnknownMode(modeString);
    ShowAvailableModes(modes, modesCount);
    return FAILURE;
}

static bool IsMode(const cc_string* argument) {
    cc_string modePrefix = String_FromConst(":");
    return String_CaselessStarts(argument, &modePrefix);
}

bool TryParseModeBlock(const cc_string* modes, const int modesCount, const cc_string* args, const int argsCount,
                       int* mode, BlockID* block) {
    *mode = 0;
    *block = Inventory_SelectedBlock;

    bool noArguments = argsCount == 0;
    bool justMode = (argsCount == 1) && IsMode(&args[0]);
    bool justBlock = (argsCount == 1) && !IsMode(&args[0]);
    bool modeAndBlock = (argsCount == 2);

    if (noArguments) {
        return SUCCESS;
    }
    else if (justMode) {
        return TryParseMode(modes, modesCount, &args[0], mode);
    }
    else if (justBlock) {
        return TryParseBlock(&args[0], block);
    }
    else if (modeAndBlock) {
        return (TryParseMode(modes, modesCount, &args[0], mode) && TryParseBlock(&args[1], block));
    }

    PlayerMessage("&fThis command only takes two arguments.");
    return FAILURE;
}
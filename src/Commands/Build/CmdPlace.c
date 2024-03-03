#include <stdio.h>

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Inventory.h"
#include "ClassiCube/src/Constants.h"

#include "MarkSelection.h"
#include "Message.h"
#include "Draw.h"
#include "Parse.h"
#include "Player.h"

static void Place_Command(const cc_string* args, int argsCount);
static void Place(IVec3 position, BlockID block);

struct ChatCommand PlaceCommand = {
    "Place",
    Place_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Place [coordinates]",
        "&fPlaces a block &fat &bcoordinates&f.",
        "\x07 &bcoordinates&f: three space-separated integers &f(defaults to current position).",
        NULL,
        NULL
    },
    NULL
};

static void Place(IVec3 position, BlockID block) {
    cc_string blockName = Block_UNSAFE_GetName(block);
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    Draw_Start("Place");
    Draw_Block(position.X, position.Y, position.Z, block);
    int blocksAffected = Draw_End();

    if (blocksAffected == 1) {
        String_Format4(&message, "&b%s &fblock was placed at &b(%i, %i, %i)&f.", &blockName, &position.X, &position.Y, &position.Z);
    } else {
        String_Format4(&message, "&b(%i, %i, %i)&f is already &b%s&f.", &position.X, &position.Y, &position.Z, &blockName);
    }

    Chat_Add(&message);
}

static void Place_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0 && argsCount != 3) {
        Message_CommandUsage(PlaceCommand);
        return;
    }

    BlockID block = Inventory_SelectedBlock;
    IVec3 position = SnapToWorldBoundaries(Player_GetPosition());

    bool hasCoordinates = (argsCount == 3);

    if (hasCoordinates && !Parse_TryParseCoordinates(&args[0], &position)) {
        return;
    }

    Place(position, block);
}

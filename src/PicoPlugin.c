#ifdef _WIN32
    #define CC_API __declspec(dllimport)
    #define CC_VAR __declspec(dllimport)
    #define EXPORT __declspec(dllexport)
#else
    #define CC_API
    #define CC_VAR
    #define EXPORT __attribute__((visibility("default")))
#endif

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/Entity.h"
#include "ClassiCube/src/Event.h"
#include "ClassiCube/src/Server.h"
#include "ClassiCube/src/BlockPhysics.h"
#include "ClassiCube/src/Commands.h"

#include "Commands/Commands.h"
#include "UndoTree.h"
#include "Message.h"
#include "MarkSelection.h"

static void Pico_Init(void) {
    String_AppendConst(&Server.AppName, " + Pico 0.3.1");

    if (!Server.IsSinglePlayer) {
        return;
    }

    Commands_RegisterAll();

    // Disables physics as they are incompatible with undo.    
    Physics.Enabled = false;

    Message_Player("Plugin &bPico 0.3.1 &fwas loaded. Get started with &b/Pico&f.");
}

static void Pico_Free(void) {
    if (!Server.IsSinglePlayer) {
        return;
    }

    // TODO: Find a way to unregister commands.

    // Abort selection in case there is currently a selection in progress.
    MarkSelection_Abort();
    UndoTree_Disable();
}

static void Pico_OnNewMapLoaded(void) {
    if (!Server.IsSinglePlayer) {
        return;
    }

    // Clears the undo tree when loading a new map.
    UndoTree_Disable();
    UndoTree_Enable();

    // Abort selection in case there is currently a selection in progress.
    MarkSelection_Abort();
}

EXPORT int Plugin_ApiVersion = 1;
EXPORT struct IGameComponent Plugin_Component = {
    .Init = Pico_Init,
    .Free = Pico_Free,
    .Reset = NULL,
    .OnNewMap = NULL,
    .OnNewMapLoaded = Pico_OnNewMapLoaded,
};

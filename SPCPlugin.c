#ifdef _WIN32
    #define CC_API __declspec(dllimport)
    #define CC_VAR __declspec(dllimport)
    #define EXPORT __declspec(dllexport)
#else
    #define CC_API
    #define CC_VAR
    #define EXPORT __attribute__((visibility("default")))
#endif

#include "CC_API/Chat.h"
#include "CC_API/Game.h"
#include "CC_API/Entity.h"
#include "CC_API/Event.h"
#include "CC_API/Server.h"

#include "SPCCommand.h"
#include "UndoTree.h"
#include "MarkSelection.h"

static void OnChatSending(void* obj, const cc_string* msg, int msgType) {
    const cc_string clientCuboid = String_FromReadonly("/client cuboid");
    const cc_string cuboid = String_FromReadonly("/cuboid");

	cc_string text;

    if (String_CaselessStarts(msg, &clientCuboid) || String_CaselessStarts(msg, &cuboid)) {
        text = String_FromReadonly("&cWarning. &fYou are using the vanilla &b/Cuboid&f.");
        Chat_Add(&text);
        text = String_FromReadonly("&fYou won't be able to &b/Undo&f, &b/Mark&f or &b/Abort&f. Use &b/Z &finstead.");
        Chat_Add(&text);
    }
}

static void OnMapLoaded(void* obj) {
	if (UndoTree_Enabled()) {
		UndoTree_Disable();
	}

	UndoTree_Enable();
}

static void RegisterChatSending() {
    struct Event_Void* event = (struct Event_Void*) &ChatEvents.ChatSending;
    Event_Void_Callback callback = (Event_Void_Callback)OnChatSending;
    Event_Register(event, NULL, callback);
}
static void EnableUndoWhenMapLoaded() {
    struct Event_Void* event = (struct Event_Void*) &WorldEvents.MapLoaded;
    Event_Void_Callback callback = (Event_Void_Callback)OnMapLoaded;
    Event_Register(event, NULL, callback);
}

static void SinglePlayerCommandsPlugin_Init(void) {
	if (!Server.IsSinglePlayer) {
		return;
	}

	SPCCommand_RegisterAll();
	RegisterChatSending();
	EnableUndoWhenMapLoaded();
	MarkSelection_Abort();
}

EXPORT int Plugin_ApiVersion = 1;
EXPORT struct IGameComponent Plugin_Component = { SinglePlayerCommandsPlugin_Init };

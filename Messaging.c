#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/String.h"

void Message_Player(const char* message) {
	cc_string ccStringMessage = String_FromReadonly(message);
	Chat_Add(&ccStringMessage);
}

void Message_MessageOf(const char* message, int messageType) {
	cc_string ccStringMessage = String_FromReadonly(message);
	Chat_AddOf(&ccStringMessage, messageType);
}

void Message_BlocksAffected(int blocksAffected) {
	char message[64];

	if (blocksAffected == 0) {
		snprintf(message, sizeof(message), "No blocks affected.");
	} else if (blocksAffected == 1) {
		snprintf(message, sizeof(message), "&b%d&f block affected.", blocksAffected);
	} else {
		snprintf(message, sizeof(message), "&b%d&f blocks affected.", blocksAffected);
	}

	Message_Player(message);
}

void Message_ShowUnknownMode(const cc_string* mode) {
    char message[64];
    cc_string cc_message = { message, .length = 0, .capacity = sizeof(message) };
    String_Format1(&cc_message, "&fUnkown mode: &b%s&f.", mode);
    Chat_Add(&cc_message);
}

void Message_ShowAvailableModes(const cc_string* modes, int modesCount) {
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

void Message_ShowUnknownBlock(const cc_string* block) {
    char buffer[128];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_Format1(&message, "&fCould not find block specified: &b%s&f.", block);
    Chat_Add(&message);
}

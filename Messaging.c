#include <stdio.h>

#include "CC_API/Chat.h"
#include "CC_API/String.h"

#include "Messaging.h"

void PlayerMessage(const char* message) {
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

	PlayerMessage(message);
}

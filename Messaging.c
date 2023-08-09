#include "CC_API/Chat.h"
#include "CC_API/String.h"

#include "Messaging.h"

void PlayerMessage(const char* message)
{
	cc_string ccStringMessage = String_FromReadonly(message);
	Chat_Add(&ccStringMessage);
}

#ifndef SPC_COMMAND_H
#define SPC_COMMAND_H

#include "CC_API/String.h"
#include "CC_API/Chat.h"

typedef struct SPCCommand_ {
	struct ChatCommand* chatCommand;
	bool canStatic;
} SPCCommand;

void SPCCommand_RegisterAll();
SPCCommand* SPCCommand_Find(const cc_string* commandName);

#endif /* SPC_COMMAND_H */

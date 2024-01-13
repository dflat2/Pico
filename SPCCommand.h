#ifndef SPC_COMMAND_H
#define SPC_COMMAND_H

#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Chat.h"

typedef struct SPCCommand_ {
	struct ChatCommand* chatCommand;
	bool canStatic;
} SPCCommand;

void SPCCommand_RegisterAll();
SPCCommand* SPCCommand_Find(const cc_string* commandName);

#endif /* SPC_COMMAND_H */

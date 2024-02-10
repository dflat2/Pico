#ifndef MESSAGING_H
#define MESSAGING_H

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/String.h"

void Message_BlocksAffected(int blocksAffected);
void Message_ShowAvailableModes(const cc_string* modes, int modesCount);
void Message_ShowUnknownMode(const cc_string* mode);
void Message_Player(const char* message);
void Message_ShowUnknownBlock(const cc_string* block);
void Message_ShowInvalidAxis(const cc_string* axis);
void Message_ShowInvalidDegrees(const cc_string* string);
void Message_UndoCheckedOut(int commit, int timestamp);
void Message_CommandUsage(struct ChatCommand command);
void Message_MemoryError(const char* action);

#endif

#ifndef MESSAGING_H
#define MESSAGING_H

#include "CC_API/String.h"

void Message_BlocksAffected(int blocksAffected);
void Message_MessageOf(const char* message, int messageType);
void Message_ShowAvailableModes(const cc_string* modes, int modesCount);
void Message_ShowUnknownMode(const cc_string* mode);
void PlayerMessage(const char* message);
void Message_ShowUnknownBlock(const cc_string* block);

#endif /* MESSAGING_H */

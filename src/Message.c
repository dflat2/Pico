#include <stdio.h>

#include "ClassiCube/src/Chat.h"
#include "ClassiCube/src/String.h"
#include "ClassiCube/src/Commands.h"

#include "Format.h"

void Message_Player(const char* message) {
    cc_string ccStringMessage = String_FromReadonly(message);
    Chat_Add(&ccStringMessage);
}

void Message_Selection(const char* message) {
    cc_string ccStringMessage = String_FromReadonly(message);
    Chat_AddOf(&ccStringMessage, MSG_TYPE_CLIENTSTATUS_1);
}

void Message_BlocksAffected(int blocksAffected) {
    char messageBuffer[STRING_SIZE];
    cc_string message = String_FromArray(messageBuffer);

    char countBuffer[STRING_SIZE];
    cc_string count = String_FromArray(countBuffer);

    Format_Int32(&count, blocksAffected);

    if (blocksAffected == 0) {
        String_AppendConst(&message, "No blocks affected.");
    } else if (blocksAffected == 1) {
        String_Format1(&message, "&b%s&f block affected.", &count);
    } else {
        String_Format1(&message, "&b%s&f blocks affected.", &count);
    }

    Chat_Add(&message);
}

void Message_ShowUnknownMode(const cc_string* mode) {
    char message[STRING_SIZE];
    cc_string cc_message = { message, .length = 0, .capacity = sizeof(message) };
    String_Format1(&cc_message, "Unkown mode: &b%s&f.", mode);
    Chat_Add(&cc_message);
}

void Message_ShowAvailableModes(const cc_string* modes, int modesCount) {
    char buffer[STRING_SIZE];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_AppendConst(&message, "Available modes: &b");

    for (int i = 0; i < modesCount - 1; i++) {
        String_AppendString(&message, &modes[i]);
        String_AppendConst(&message, "&f, &b");
    }

    String_AppendString(&message, &modes[modesCount - 1]);
    String_AppendConst(&message, "&f.");
    Chat_Add(&message);
}

void Message_ShowUnknownBlock(const cc_string* block) {
    char buffer[STRING_SIZE];
    cc_string message = { buffer, .length = 0, .capacity = sizeof(buffer) };
    String_Format1(&message, "Could not find block specified: &b%s&f.", block);
    Chat_Add(&message);
}

void Message_ShowInvalidAxis(const cc_string* axis) {
    char buffer[STRING_SIZE];
    cc_string message = String_FromArray(buffer);
    String_Format1(&message, "Could not read axis: &b%s&f.", axis);
    Chat_Add(&message);
    Message_Player("An axis parameter must be &bX&f, &bY&f or &bZ&f.");
}

void Message_ShowInvalidDegrees(const cc_string* string) {
    char buffer[STRING_SIZE];
    cc_string message = String_FromArray(buffer);
    String_Format1(&message, "Invalid degrees: &b%s&f.", string);
    Chat_Add(&message);
    Message_Player("Degrees must be integers, multiples of 90.");
}

void Message_UndoCheckedOut(int commit, int timestampInt) {
    char buffer_timestamp[] = "00:00:00";
    cc_string timestamp = String_FromArray(buffer_timestamp);
    Format_HHMMSS(&timestamp, timestampInt);

    char successMessageBuffer[STRING_SIZE];
    cc_string successMessage = String_FromArray(successMessageBuffer);

    String_Format2(&successMessage, "Checked out operation &b%i&f [&b%s&f].", &commit, &timestamp);
    Chat_Add(&successMessage);
}

void Message_CommandUsage(struct ChatCommand command) {
    const char* firstHelpLine = command.help[0];
    char usageMessage[65];
    snprintf(usageMessage, sizeof(usageMessage), "Usage: %s&f.", firstHelpLine);
    Message_Player(usageMessage);
}


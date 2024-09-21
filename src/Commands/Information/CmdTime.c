#include <time.h>
#include "ClassiCube/src/Constants.h"
#include "ClassiCube/src/Chat.h"
#include "Message.h"

static void Time_Command(const cc_string* args, int argsCount);

struct ChatCommand TimeCommand = {
    "Time",
    Time_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Time",
        "Displays current date and time.",
        NULL,
        NULL,
        NULL
    },
    NULL
};

static void Time_Command(const cc_string* args, int argsCount) {
    if (argsCount != 0) {
        Message_CommandUsage(TimeCommand);
        return;
    }

    time_t now = time(NULL);
    struct tm* timeStruct = localtime(&now);
    char timeStringBuffer[STRING_SIZE];
    cc_string timeString = String_FromArray(timeStringBuffer);

    timeString.length = strftime(timeString.buffer, timeString.capacity, "Current time: &b%X &fon &b%Y-%m-%d&f.", timeStruct);
    Chat_Add(&timeString);
}

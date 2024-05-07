#include "err-msg.h"

const char *appStrError(
    int code
)
{
    switch (code) {
        case ERR_CODE_COMMAND_LINE:
            return "Invalid command liner argument(s)";
        case ERR_CODE_SOCKET_BIND:
            return "Bind socket error";
        case ERR_CODE_SOCKET_LISTEN:
            return "Listen socket error";
        case ERR_CODE_STOPPED:
            return "Stopped";
        default:
            return "";
    }
}

#ifndef SIMPLE_PRESENSE_SERVICE_PRESENCE_ITEM_H
#define SIMPLE_PRESENSE_SERVICE_PRESENCE_ITEM_H

#include <cinttypes>
#include <time.h>
#include <sys/socket.h>

class PresenceItem {
public:
    time_t time;
    uint64_t uid;
    uint64_t accessCode;
    struct sockaddr addr;
    PresenceItem();
    virtual ~PresenceItem();
};

#endif

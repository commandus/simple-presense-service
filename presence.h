#ifndef SIMPLE_PRESENSE_SERVICE_PRESENCE_H
#define SIMPLE_PRESENSE_SERVICE_PRESENCE_H

#include <cinttypes>

#include "presence-item.h"

class Presence {
public:
    Presence();
    virtual ~Presence();
    virtual bool get(PresenceItem &item) = 0;
    virtual void put(const PresenceItem &item) = 0;

    unsigned int query(
        const struct sockaddr *addr,
        unsigned char *retBuf, std::size_t retBufSize,
        const unsigned char* buf, std::size_t bufSize
    );
};

#endif

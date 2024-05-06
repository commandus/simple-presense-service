#ifndef SIMPLE_PRESENSE_SERVICE_PRESENCE_H
#define SIMPLE_PRESENSE_SERVICE_PRESENCE_H

#include <cinttypes>

#include "presence-item.h"

class Presence {
public:
    Presence();
    virtual ~Presence();
    virtual bool get(PresenceItem &item) = 0;
    virtual void set(const PresenceItem &item) = 0;

    std::size_t query(
        const struct sockaddr *addr,
        unsigned char *retBuf, std::size_t retBufSize,
        const unsigned char* buf, std::size_t bufSize
    );
};

#endif

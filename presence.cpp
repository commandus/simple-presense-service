#include <cstring>
#include "presence.h"

Presence::Presence() = default;
Presence::~Presence() = default;

/*
* Request <SRC-UID:16>[<DEST-UID:16>] = 16, 32 bytes
* Response <UID:16><ADDR:16> = 32 bytes
*/
unsigned int Presence::query(
    const struct sockaddr* addr,
    unsigned char* retBuf, std::size_t retBufSize,
    const unsigned char* buf, std::size_t bufSize
)
{
    if (bufSize == 16) {
        // return source "external" address
        // pong source uid
        memmove(retBuf, buf, sizeof(UID));
        // return my address as visible for outside
        memmove(retBuf + sizeof(UID), addr, sizeof(struct sockaddr));
        PresenceItem item((UID*)buf, addr);
        put(item);
        return 32;
    }
    if (bufSize == 32) {
        // update source address
        PresenceItem item((UID*)buf, addr);
        put(item);
        // pong dest uid
        memmove(retBuf, buf + sizeof(UID), sizeof(UID));
        // request dest by uid
        memmove(&item.uid, buf + sizeof(UID), sizeof(UID));
        if (!get(item))
            return 0;
        // return address
        memmove(retBuf + sizeof(UID), &item.addr, sizeof(struct sockaddr));
        return 32;
    }
    return 0;
}

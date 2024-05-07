#ifndef SIMPLE_PRESENSE_SERVICE_PRESENCE_ITEM_H
#define SIMPLE_PRESENSE_SERVICE_PRESENCE_ITEM_H

#include <chrono>

typedef std::chrono::time_point<std::chrono::system_clock> TASK_TIME;

#include <cinttypes>
#include <ctime>
#ifdef _MSC_VER
#include <WinSock2.h>
#include <guiddef.h>
#else
#include <sys/socket.h>
#include <string>

#endif

class UID {
public:
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];

    UID();
    UID(const char *s);
    bool operator <(const UID &rhs) const;
    UID& operator =(const char *);
    bool parseString(const char *);
    std::string toString() const;
    void generateRandom();
};

class PresenceItem {
public:
    TASK_TIME modified;
    UID uid;
    struct sockaddr addr; // IPv4 16 bytes.
    PresenceItem();
    PresenceItem(const UID *uid, const struct sockaddr *addr);
    virtual ~PresenceItem();
};

#endif

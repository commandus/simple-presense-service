//
// Created by andrei on 06.05.24.
//

#ifndef SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H
#define SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H

#include <map>
#include "presence.h"

class PresenceItemRec {
public:
    TASK_TIME modified;
    struct sockaddr addr;   // IPv4: 8 bytes, up to 16 bytes.
    bool expired(const TASK_TIME &since) const;
};

class MemPresence : public Presence {
private:
    std::map<UID, PresenceItemRec> storage;
    void cleanExpired(const TASK_TIME &now);
public:
    MemPresence();
    virtual ~MemPresence();
    bool get(PresenceItem &item) override;
    void put(const PresenceItem &item) override;
};

#endif //SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H

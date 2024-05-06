//
// Created by andrei on 06.05.24.
//

#ifndef SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H
#define SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H

#include "presence.h"

class MemPresence : public Presence {
public:
    MemPresence();
    virtual ~MemPresence();
    bool get(PresenceItem &item) override;
    void put(const PresenceItem &item) override;
};

#endif //SIMPLE_PRESENSE_SERVICE_MEM_PRESENCE_H

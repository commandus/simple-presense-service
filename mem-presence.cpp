#include <cstring>
#include "mem-presence.h"

#define DEF_EXPIRATION_SEC  180

bool PresenceItemRec::expired(
    const TASK_TIME &since
) const
{
    return (std::chrono::duration_cast<std::chrono::seconds>(since - modified).count() > DEF_EXPIRATION_SEC);
}

MemPresence::MemPresence() = default;

MemPresence::~MemPresence() = default;

void MemPresence::cleanExpired(
    const TASK_TIME &now
)
{
    for (auto it(storage.begin()); it != storage.end(); it++) {
        if (it->second.expired(now))
            it = storage.erase(it);
    }
}

bool MemPresence::get(
    PresenceItem &item
)
{
    TASK_TIME now = std::chrono::system_clock::now();
    auto it = storage.find(item.uid);
    if (it == storage.end())
        return false;
    if (it->second.expired(now)) {
        storage.erase(it);
        return false;
    }
    memmove(&item.addr, &it->second.addr, sizeof(struct sockaddr));
    item.modified = it->second.modified;
    return true;
}

void MemPresence::put(
    const PresenceItem &item
)
{
    TASK_TIME now = std::chrono::system_clock::now();
    PresenceItemRec r {item.modified, item.addr };
    storage[item.uid] = r;
    cleanExpired(now);
}

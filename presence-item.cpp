#include "presence-item.h"

PresenceItem::PresenceItem()
	: modified(std::chrono::system_clock::now())
{

}

PresenceItem::PresenceItem(
	const UID* aUid, 
	const struct sockaddr* aAddr
)
	: modified(std::chrono::system_clock::now())
{
	memmove(&uid, aUid, sizeof(UID));
	memmove(&addr, aAddr, sizeof(struct sockaddr));
}

PresenceItem::~PresenceItem() = default;

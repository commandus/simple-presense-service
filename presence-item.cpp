#include <cstring>
#include <ios>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <random>
#include "presence-item.h"

static bool isHexChar(
    char ch
)
{
    // 0-9
    if (ch > 47 && ch < 58)
        return true;
    // a-f
    if (ch > 96 && ch < 103)
        return true;
    // A-F
    if (ch > 64 && ch < 71)
        return true;
    return false;
}

static unsigned char hexDigitToChar(
    char ch
)
{
    // 0-9
    if (ch > 47 && ch < 58)
        return ch - 48;
    // a-f
    if (ch > 96 && ch < 103)
        return ch - 87;
    // A-F
    if (ch > 64 && ch < 71)
        return ch - 55;
    return 0;
}

static unsigned char hex2char(
    char *digits
)
{
    return hexDigitToChar(digits[0]) * 16 + hexDigitToChar(digits[1]);
}

UID::UID()
{
    memset(&data1, 16, 0);
}

UID::UID(const char *s)
{
    parseString(s);
}

bool UID::parseString(
    const char *s
)
{
    const char *p = s;
    int digitCount = 0;
    int bytes = 0;
    char a[2];
    unsigned char *c = (unsigned char *) &data1;
    while (*p) {
        if (isHexChar(*p)) {
            digitCount++;
            if (digitCount == 1) {
                a[0] = *p;
            } else {
                if (digitCount == 2) {
                    a[1] = *p;
                    if (bytes > 15)
                        break;
                    c[bytes] = hex2char(a);
                    bytes++;
                    digitCount = 0;
                }
            }
        } else {
            digitCount = 0;
        }
        p++;
    }
    return bytes >= 15;
}

std::string UID::toString() const
{
    std::stringstream ss;
    std::ios_base::fmtflags f(ss.flags());
    unsigned char *c = (unsigned char *) &this->data1;
    ss << std::hex << std::setfill('0')
        << std::setw(2) << (int) c[0]
        << std::setw(2) << (int) c[1]
        << std::setw(2) << (int) c[2]
        << std::setw(2) << (int) c[3]
        << "-"
        << std::setw(2) << (int) c[4]
        << std::setw(2) << (int) c[5]
        << "-"
        << std::setw(2) << (int) c[6]
        << std::setw(2) << (int) c[7]
        << "-"
        << std::setw(2) << (int) c[8]
        << std::setw(2) << (int) c[9]
        << "-"
        << std::setw(2) << (int) c[10]
        << std::setw(2) << (int) c[11]
        << std::setw(2) << (int) c[12]
        << std::setw(2) << (int) c[13]
        << std::setw(2) << (int) c[14]
        << std::setw(2) << (int) c[15];
    ss.flags(f);
    return ss.str();
}

void UID::generateRandom()
{
    // sorry
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, UINT64_MAX);

    long *r = (long *) &data1;
    for (int i = 0; i < 4; i++) {
        r[i] = dist(rng);
    }
    unsigned char *c = (unsigned char *) &this->data1;
}

bool UID::operator <(
    const UID &rhs
) const
{
    return memcmp(this, &rhs, sizeof(UID)) < 0;
}

UID& UID::operator =(
    const char *s
)
{
    parseString(s);
    return *this;
}

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

PresenceItem::PresenceItem(
    void* buffer
)
    : modified(std::chrono::system_clock::now())
{
    memmove(&uid, buffer, sizeof(UID));
    memmove(&addr, (char *) buffer + sizeof(UID), sizeof(struct sockaddr));
}

PresenceItem::~PresenceItem() = default;

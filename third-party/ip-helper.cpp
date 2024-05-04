#include "ip-helper.h"

#ifdef _MSC_VER
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

bool isAddrStringIPv6(
    const char * value
) {
    struct in6_addr result = {};
    return inet_pton(AF_INET6, value, &result) == 1;
}

bool isIPv6(
    const struct sockaddr *addr
) {
    return addr->sa_family == AF_INET6;
}

bool isIP(
    const struct sockaddr *addr
)
{
    return addr->sa_family == AF_INET || addr->sa_family == AF_INET6;
}

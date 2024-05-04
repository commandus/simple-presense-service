#ifndef IP_HELPER_H
#define IP_HELPER_H

bool isAddrStringIPv6(
    const char *hostAddr
);

bool isIPv6(
    const struct sockaddr *addr
);

bool isIP(
    const struct sockaddr *addr
);

#endif
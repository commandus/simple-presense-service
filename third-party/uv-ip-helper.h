#ifndef UV_IP_HELPER_H
#define UV_IP_HELPER_H

#include <string>
#include <uv.h>

void getAddrNPort(
    uv_tcp_t *stream,
    std::string &retName,
    int &retPort
);

/**
 * Returns IP address dotted representation, remotePort number
 * @return true: valid IP address false: address is NULL
 */
bool getSocketAddrNPort(
    const struct sockaddr *addr,
    std::string &retHost,
    int &retPort
);

#endif
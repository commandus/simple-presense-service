#include "uv-ip-helper.h"

#ifdef _MSC_VER
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

void getAddrNPort(
    uv_tcp_t *stream,
    std::string &retName,
    int &retPort
)
{
    struct sockaddr_storage name{};
    auto *n = (struct sockaddr *)&name;
    int nameSize = sizeof(name);
    if (uv_tcp_getpeername(stream, n, &nameSize)) {
        retName = "";
        retPort = 0;
        return;
    }
    char addr[INET6_ADDRSTRLEN];
    if (name.ss_family == AF_INET) {
        uv_inet_ntop(name.ss_family, &((struct sockaddr_in *)n)->sin_addr, addr, sizeof(addr));
        retPort = ntohs(((struct sockaddr_in *)n)->sin_port);
    } else {
        uv_inet_ntop(name.ss_family, &((struct sockaddr_in6 *)n)->sin6_addr, addr, sizeof(addr));
        retPort = ntohs(((struct sockaddr_in6 *)n)->sin6_port);
    }
    retName = addr;
}

/**
 * Returns IP address dotted representation, port number
 * @return true: valid IP address false: address is NULL
 */
bool getSocketAddrNPort(
    const struct sockaddr *addr,
    std::string &retHost,
    int &retPort
)
{
    if (!addr) {
        retHost = "";
        retPort = 0;
        return false;
    }

    auto sin = (struct sockaddr_in *)addr;
    char ip[INET6_ADDRSTRLEN];
    inet_ntop(addr->sa_family, &sin->sin_addr, ip, sizeof(ip));
    retHost = std::string(ip);
    retPort = ntohs(sin->sin_port);
    return true;
}

#ifndef UV_LISTENER_H_
#define UV_LISTENER_H_	1

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#include <string>
#ifdef _MSC_VER
#else
#include <arpa/inet.h>
#include <uv.h>

#endif

#include "presence.h"

class UVClient {
private:
    // libuv handler
    void *uv;
    struct sockaddr servaddr;
public:
    int verbose;
    Presence *presence;
    int status;
    struct sockaddr remoteAddress;
    uv_udp_t udpSocket;

    UVClient();
    virtual ~UVClient();

    void setRemoteAddress(
        const std::string &host,
        uint16_t port
    );
    void setAddress(
        const std::string &host,
        uint16_t port
    );
    void setAddress(
        uint32_t &ipv4,
        uint16_t port
    );
    int run();
    void stop();
};

#endif

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
#endif

class UVListener {
private:
    // libuv handler
    void *uv;
    struct sockaddr servaddr;
    int verbose;
public:
    int status;
    UVListener();
    virtual ~UVListener();
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

    size_t query(unsigned char *retBuf, size_t retBufSize, const unsigned char* buf, size_t bufSize);

};

#endif

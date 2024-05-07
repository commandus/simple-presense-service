#include "uv-listener.h"

#include <algorithm>
#include <iostream>
#include <uv.h>

#ifdef _MSC_VER
#include <io.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define write _write
#define close _close 
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#define SOCKET int
#endif

#include "uv-mem.h"
#include "ip-helper.h"
#include "uv-ip-helper.h"
#include "err-msg.h"
#include "mem-presence.h"
#include <ip-address.h>

// i18n
// #include <libintl.h>
// #define _(String) gettext (String)
#define _(String) (String)

#define DEF_TCP_KEEPALIVE_SECS 60

#define WRITE_BUFFER_SIZE 1024

static void onUDPRead(
	uv_udp_t *handle,
	ssize_t bytesRead,
	const uv_buf_t *buf,
	const struct sockaddr *addr,
	unsigned flags
)
{
    UVListener* listener = (UVListener*) handle->loop->data;
    if (!listener)
        return;
  	if (bytesRead < 0) {
    	if (bytesRead != UV_EOF) {
    	}
  	} else {
        if (bytesRead == 0) {

        } else {
            unsigned char writeBuffer[32];
            unsigned int sz = listener->presence->query(
                addr, writeBuffer, sizeof(writeBuffer),
                (const unsigned char *) buf->base, bytesRead);
            if (sz > 0) {
                if (listener->verbose > 1) {
                    PresenceItem it(&writeBuffer);
                    std::cout
                        << it.uid.toString() << ": "
                        << sockaddr2string(&it.addr)
                        << std::endl;
                }
                uv_buf_t wrBuf = uv_buf_init((char *) writeBuffer, sz);
                auto req = (uv_udp_send_t *) malloc(sizeof(uv_udp_send_t));
				if (req) {
					req->data = writeBuffer; // to free up if need it
					uv_udp_send(req, handle, &wrBuf, 1, addr,
						[] (uv_udp_send_t* req, int status) {
							if (req)
								free(req);
					});
				}
            }
        }
    }
	freeBuffer(buf);
}

static void onReadTCP(
	uv_stream_t *client,
	ssize_t readCount,
	const uv_buf_t *buf
)
{
	if (readCount <= 0) {
		if (readCount != UV_EOF) {
		}
		// client disconnected, close socket
		uv_close((uv_handle_t *)client, onCloseClient);
	} else {
        unsigned char writeBuffer[WRITE_BUFFER_SIZE];
        sockaddr addr;
        int addrLen = sizeof(sockaddr);
        uv_tcp_getpeername((const uv_tcp_t *) client, &addr, &addrLen);
        unsigned int sz = ((UVListener*) client->loop->data)->presence->query(
            &addr, writeBuffer, sizeof(writeBuffer),
            (const unsigned char *) buf->base, readCount
        );
        if (sz > 0) {
			uv_write_t *req = allocReq();
			uv_buf_t writeBuf = uv_buf_init((char *) writeBuffer, sz);
			req->data = writeBuffer; // to free up if required
			uv_write(req, client, &writeBuf, 1,
                 [](uv_write_t *req, int status) {
                     if (req)
                         free(req);
                }
            );
        }
        bool keepalive = true;
        if (!keepalive)
            uv_close((uv_handle_t *)client, onCloseClient);
	}
    freeBuffer(buf);
}

static void onConnect(
    uv_stream_t *server,
    int status
)
{
	if (status < 0) {
#ifdef ENABLE_DEBUG		
		std::cerr << ERR_SOCKET_CREATE << status << std::endl;
#endif		
		return;
	}
	uv_tcp_t *client = allocClient();
	uv_tcp_init(server->loop, client);
    uv_tcp_keepalive(client, 1, DEF_TCP_KEEPALIVE_SECS);
#ifdef ENABLE_DEBUG
    std::cerr << MSG_CONNECTED << std::endl;
#endif
    if (uv_accept(server, (uv_stream_t *)client) == 0) {
		uv_read_start((uv_stream_t *)client, allocBuffer, onReadTCP);
	} else {
		uv_close((uv_handle_t *)client, onCloseClient);
	}
}

/**
 * @see https://habr.com/ru/post/340758/
 */
UVListener::UVListener()
	: serviceAddress{}, verbose(0), presence(new MemPresence), status(CODE_OK)
{
	uv_loop_t *loop = uv_default_loop();
    loop->data = this;
    uv = (void *) loop;
}

// http://stackoverflow.com/questions/25615340/closing-libuv-handles-correctly
void UVListener::stop()
{
	auto *uvLoop = (uv_loop_t *) uv;
    if (!uvLoop || status == ERR_CODE_STOPPED)
        return;
    status = ERR_CODE_STOPPED;
    uv_stop(uvLoop);
    int result = uv_loop_close(uvLoop);

	if (result == UV_EBUSY && uv_loop_alive(uvLoop)) {
		uv_walk(uvLoop, [](uv_handle_t* handle, void* arg) {
			if (handle->loop && !uv_is_closing(handle))
				uv_close(handle, nullptr);
			}, nullptr);
		int r;
		do {
			r = uv_run(uvLoop, UV_RUN_ONCE);
		} while (r != 0);
        uv_loop_close(uvLoop);
	}
}

void UVListener::setAddress(
    const std::string &host,
    uint16_t port
)
{
    if (isAddrStringIPv6(host.c_str()))
        uv_ip6_addr(host.c_str(), port, (sockaddr_in6*) &serviceAddress);
    else
        uv_ip4_addr(host.c_str(), port, (sockaddr_in*) &serviceAddress);
}

void UVListener::setAddress(
    uint32_t &ipv4,
    uint16_t port
)
{
	struct sockaddr_in *a = (struct sockaddr_in *) &serviceAddress;
    a->sin_family = AF_INET;
    a->sin_addr.s_addr = ipv4;
    a->sin_port = htons(port);
}

int UVListener::run()
{
	auto *loop = (uv_loop_t *) uv;
	// TCP
	uv_tcp_t tcpSocket;
	uv_tcp_init(loop, &tcpSocket);
    uv_tcp_keepalive(&tcpSocket, 1, DEF_TCP_KEEPALIVE_SECS);
	uv_tcp_bind(&tcpSocket, (const struct sockaddr *)&serviceAddress, 0);
	int r = uv_listen((uv_stream_t *) &tcpSocket, 128, onConnect);
	if (r) {
#ifdef ENABLE_DEBUG
		std::cerr << ERR_SOCKET_LISTEN << uv_strerror(r) << std::endl;
#endif		
		status = ERR_CODE_SOCKET_LISTEN;
		return status;
	}

	// UDP
	uv_udp_t udpSocket;
	uv_udp_init(loop, &udpSocket);
	r = uv_udp_bind(&udpSocket, (const struct sockaddr *)&serviceAddress, UV_UDP_REUSEADDR);
    if (r) {
#ifdef ENABLE_DEBUG
        std::cerr << ERR_SOCKET_BIND << uv_strerror(r) << std::endl;
#endif
        status = ERR_CODE_SOCKET_BIND;
        return status;
    }
	uv_udp_set_broadcast(&udpSocket, 1);
	r = uv_udp_recv_start(&udpSocket, allocBuffer, onUDPRead);
	if (r) {
#ifdef ENABLE_DEBUG
		std::cerr << ERR_SOCKET_LISTEN << uv_strerror(r) << std::endl;
#endif
		status = ERR_CODE_SOCKET_LISTEN;
		return ERR_CODE_SOCKET_LISTEN;
	}
	status = CODE_OK;
	uv_run(loop, UV_RUN_DEFAULT);
    return status;
}

/**
 * 	Call stop() before destroy
 */
UVListener::~UVListener()
{
    if (presence)
        delete presence;
    presence = nullptr;
}

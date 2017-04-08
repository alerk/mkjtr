/*
 * LocalResolv.cpp
 *
 *  Created on: 30. 6. 2016
 *      Author: sevcikr
 */

#include "LocalResolv.h"

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#endif

#include <cstdio>

/* ==================================================================== */
struct addrinfop_wrapper {
	addrinfop_wrapper()
	  : ai(nullptr)
	{ }

	~addrinfop_wrapper() {
        if (ai != nullptr)
		    ::freeaddrinfo(ai);
	}

    addrinfop_wrapper(const addrinfop_wrapper&) = delete;
    addrinfop_wrapper(addrinfop_wrapper&&) = delete;
    
    addrinfop_wrapper& operator=(const addrinfop_wrapper&) = delete;
    addrinfop_wrapper& operator=(addrinfop_wrapper&&) = delete;

    struct addrinfo** operator&()
    { return &ai; }

    struct addrinfo* operator->()
    { return ai; }

private:
	struct addrinfo *ai;
};

/* ==================================================================== */
struct socket_wrapper {
#ifdef WIN32
    typedef SOCKET socket_type;
#else
    typedef int socket_type;
#endif

    static socket_type invalid;

    socket_wrapper()
        : s(invalid)
    { }

    socket_wrapper(socket_type s_)
        : s(s_)
    { }

    ~socket_wrapper() {
        if (s != invalid) {
#ifdef WIN32
            ::closesocket(s);
#else
            ::close(s);
#endif
        }
    }

    socket_wrapper(const socket_wrapper&) = delete;
    socket_wrapper(socket_wrapper&&) = delete;
    socket_wrapper& operator=(const socket_wrapper&) = delete;
    socket_wrapper& operator=(socket_wrapper&&) = delete;

    operator socket_type()
    { return s; }

    bool is_valid() const
    { return s != invalid; }

private:
    socket_type s;
};

socket_wrapper::socket_type socket_wrapper::invalid = ~0;

/* ==================================================================== */
static int s_system_error()
{
#ifdef WIN32
    int ec = ::WSAGetLastError();
    //TODO: FormatMessage()
#else
    int ec = errno;
    if (ec) fprintf(stderr, "%m\n");
#endif

    return ec;
}

/* ==================================================================== */
static int s_gai_error(int ec)
{
#ifdef WIN32
    (void)ec;
    return s_system_error();
#else
    if (EAI_SYSTEM == ec)
        return s_system_error();

    fprintf(stderr, "%s\n", ::gai_strerror(ec));
    return ec;
#endif
}

/* ==================================================================== */
int sitraffic::GetLocalEndpointIp(const char *hostname,
                                  char *out, size_t outsize)
{
    int gai_ec = 0;
    addrinfop_wrapper gai_result;

    struct addrinfo hints;
	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = nullptr;
	hints.ai_canonname = nullptr;
	hints.ai_next = nullptr;

    if (0 != (gai_ec = ::getaddrinfo(hostname, nullptr,
                                     &hints, &gai_result)))
        return (s_gai_error(gai_ec), 0);

    socket_wrapper sock(::socket(gai_result->ai_family,
                                 gai_result->ai_socktype,
                                 gai_result->ai_protocol));
    if (!sock.is_valid())
        return (s_system_error(), 0);

    if (-1 == ::connect(sock, gai_result->ai_addr,
                              (socklen_t)gai_result->ai_addrlen))
        return (s_system_error(), 0);

    struct sockaddr sa;
    socklen_t salen = sizeof(sa);
    if (-1 == ::getsockname(sock, &sa, &salen))
        return (s_system_error(), 0);

    if (0 != (gai_ec = ::getnameinfo(&sa, salen,
                                     out, (socklen_t)outsize,
                                     nullptr, 0,
                                     NI_NUMERICHOST | NI_DGRAM)))
        return (s_gai_error(gai_ec), 0);

    return 1;
}

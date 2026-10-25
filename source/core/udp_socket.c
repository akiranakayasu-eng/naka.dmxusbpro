/*
 * udp_socket.c
 * SPDX-License-Identifier: MIT
 */
#include "udp_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
/* ===================================================================== */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <mstcpip.h>

#ifndef SIO_UDP_CONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
#endif

typedef SOCKET sock_t;
#define BAD_SOCKET INVALID_SOCKET
#define close_socket closesocket
static int last_errno(void) { return WSAGetLastError(); }

static void format_error(int e, char *out, size_t outlen, const char *what)
{
    char msg[96] = "";
    size_t n;
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)e, 0, msg,
                   sizeof(msg), NULL);
    n = strlen(msg);
    while (n && (msg[n - 1] == '\r' || msg[n - 1] == '\n' || msg[n - 1] == '.'))
        msg[--n] = 0;
    snprintf(out, outlen, "%s: %s (error %d)", what, msg, e);
}

static int net_startup(void)
{
    static int started = 0;
    WSADATA wsa;
    if (started)
        return 0;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return -1;
    started = 1; /* kept for the life of the process */
    return 0;
}

#else
/* ===================================================================== */
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int sock_t;
#define BAD_SOCKET (-1)
#define close_socket close
static int last_errno(void) { return errno; }

static void format_error(int e, char *out, size_t outlen, const char *what)
{
    snprintf(out, outlen, "%s: %s (errno %d)", what, strerror(e), e);
}

static int net_startup(void) { return 0; }
#endif

struct udp_socket {
    sock_t fd;
    char err[160];
};

static void set_error(udp_socket *s, const char *what)
{
    format_error(last_errno(), s->err, sizeof(s->err), what);
}

static void to_sockaddr(const uint8_t ip[4], uint16_t port, struct sockaddr_in *sa)
{
    memset(sa, 0, sizeof(*sa));
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);
    if (ip)
        memcpy(&sa->sin_addr, ip, 4);
    else
        sa->sin_addr.s_addr = htonl(INADDR_ANY);
}

udp_socket *udp_open(const uint8_t *local_ip, uint16_t port, int shared, char *err, size_t errlen)
{
    udp_socket *s;
    struct sockaddr_in sa;
    int on = 1;

    if (net_startup() != 0) {
        if (err)
            snprintf(err, errlen, "network startup failed");
        return NULL;
    }
    s = (udp_socket *)calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    s->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s->fd == BAD_SOCKET) {
        if (err)
            format_error(last_errno(), err, errlen, "socket");
        free(s);
        return NULL;
    }
    setsockopt(s->fd, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof(on));
    if (shared) {
        setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
#ifdef SO_REUSEPORT
        setsockopt(s->fd, SOL_SOCKET, SO_REUSEPORT, (const char *)&on, sizeof(on));
#endif
    }
#ifdef _WIN32
    {
        /* don't let an ICMP "port unreachable" fail the next recvfrom */
        BOOL off = FALSE;
        DWORD ret = 0;
        WSAIoctl(s->fd, SIO_UDP_CONNRESET, &off, sizeof(off), NULL, 0, &ret, NULL, NULL);
    }
#endif
    to_sockaddr(local_ip, port, &sa);
    if (bind(s->fd, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        if (err)
            format_error(last_errno(), err, errlen, "bind");
        close_socket(s->fd);
        free(s);
        return NULL;
    }
    return s;
}

void udp_close(udp_socket *s)
{
    if (!s)
        return;
    close_socket(s->fd);
    free(s);
}

int udp_set_multicast(udp_socket *s, const uint8_t *local_ip, int ttl)
{
    unsigned char t = (unsigned char)(ttl < 1 ? 1 : ttl > 255 ? 255 : ttl);
#ifdef _WIN32
    DWORD tv = t; /* Winsock wants a DWORD here */
    if (setsockopt(s->fd, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&tv, sizeof(tv)) != 0) {
#else
    if (setsockopt(s->fd, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&t, sizeof(t)) != 0) {
#endif
        set_error(s, "multicast ttl");
        return -1;
    }
    if (local_ip) {
        struct in_addr a;
        memcpy(&a, local_ip, 4);
        if (setsockopt(s->fd, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&a, sizeof(a)) != 0) {
            set_error(s, "multicast interface");
            return -1;
        }
    }
    return 0;
}

int udp_sendto(udp_socket *s, const uint8_t ip[4], uint16_t port, const uint8_t *buf, size_t len)
{
    struct sockaddr_in sa;
    int n;
    to_sockaddr(ip, port, &sa);
    n = (int)sendto(s->fd, (const char *)buf, (int)len, 0, (struct sockaddr *)&sa, sizeof(sa));
    if (n < 0) {
        set_error(s, "send");
        return -1;
    }
    s->err[0] = 0;
    return n;
}

int udp_recvfrom(udp_socket *s, uint8_t *buf, size_t len, int timeout_ms,
                 uint8_t from_ip[4], uint16_t *from_port)
{
    struct sockaddr_in sa;
    int n;
#ifdef _WIN32
    int salen = sizeof(sa);
    fd_set rd;
    struct timeval tv;
    FD_ZERO(&rd);
    FD_SET(s->fd, &rd);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    n = select(0, &rd, NULL, NULL, &tv);
    if (n == SOCKET_ERROR) {
        set_error(s, "select");
        return -1;
    }
#else
    socklen_t salen = sizeof(sa);
    struct pollfd pfd;
    pfd.fd = s->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    n = poll(&pfd, 1, timeout_ms);
    if (n < 0) {
        if (errno == EINTR)
            return 0;
        set_error(s, "poll");
        return -1;
    }
#endif
    if (n == 0)
        return 0;
    n = (int)recvfrom(s->fd, (char *)buf, (int)len, 0, (struct sockaddr *)&sa, &salen);
    if (n < 0) {
#ifdef _WIN32
        int e = WSAGetLastError();
        if (e == WSAEMSGSIZE || e == WSAECONNRESET)
            return 0; /* oversized datagram or stray ICMP: not fatal */
#else
        if (errno == EAGAIN || errno == EINTR)
            return 0;
#endif
        set_error(s, "receive");
        return -1;
    }
    if (from_ip)
        memcpy(from_ip, &sa.sin_addr, 4);
    if (from_port)
        *from_port = ntohs(sa.sin_port);
    return n;
}

uint16_t udp_local_port(udp_socket *s)
{
    struct sockaddr_in sa;
#ifdef _WIN32
    int salen = sizeof(sa);
#else
    socklen_t salen = sizeof(sa);
#endif
    if (getsockname(s->fd, (struct sockaddr *)&sa, &salen) != 0)
        return 0;
    return ntohs(sa.sin_port);
}

const char *udp_last_error(udp_socket *s)
{
    return s ? s->err : "";
}

int net_parse_ipv4(const char *text, uint8_t ip[4])
{
    struct in_addr a;
    struct addrinfo hints, *res = NULL;
    if (!text || !*text)
        return -1;
    net_startup();
    if (inet_pton(AF_INET, text, &a) == 1) {
        memcpy(ip, &a, 4);
        return 0;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(text, NULL, &hints, &res) != 0 || !res)
        return -1;
    memcpy(ip, &((struct sockaddr_in *)res->ai_addr)->sin_addr, 4);
    freeaddrinfo(res);
    return 0;
}

void net_format_ipv4(const uint8_t ip[4], char *out, size_t outlen)
{
    snprintf(out, outlen, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

static void fill_broadcast(net_iface *f)
{
    int i;
    for (i = 0; i < 4; i++)
        f->broadcast[i] = (uint8_t)(f->ip[i] | (uint8_t)~f->netmask[i]);
}

#ifdef _WIN32
int net_list_ifaces(net_iface *out, int max)
{
    ULONG size = 15000;
    IP_ADAPTER_ADDRESSES *list = NULL, *a;
    int n = 0;
    ULONG r;
    net_startup();
    do {
        free(list);
        list = (IP_ADAPTER_ADDRESSES *)malloc(size);
        if (!list)
            return 0;
        r = GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                                 GAA_FLAG_SKIP_DNS_SERVER, NULL, list, &size);
    } while (r == ERROR_BUFFER_OVERFLOW);
    if (r != NO_ERROR) {
        free(list);
        return 0;
    }
    for (a = list; a && n < max; a = a->Next) {
        IP_ADAPTER_UNICAST_ADDRESS *u;
        if (a->OperStatus != IfOperStatusUp || a->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
            continue;
        for (u = a->FirstUnicastAddress; u && n < max; u = u->Next) {
            struct sockaddr_in *sa = (struct sockaddr_in *)u->Address.lpSockaddr;
            ULONG mask = 0;
            net_iface *f = &out[n];
            if (sa->sin_family != AF_INET)
                continue;
            memset(f, 0, sizeof(*f));
            WideCharToMultiByte(CP_UTF8, 0, a->FriendlyName, -1, f->name, sizeof(f->name), NULL, NULL);
            memcpy(f->ip, &sa->sin_addr, 4);
            ConvertLengthToIpv4Mask(u->OnLinkPrefixLength, &mask);
            memcpy(f->netmask, &mask, 4);
            fill_broadcast(f);
            n++;
        }
    }
    free(list);
    return n;
}
#else
int net_list_ifaces(net_iface *out, int max)
{
    struct ifaddrs *list = NULL, *a;
    int n = 0;
    if (getifaddrs(&list) != 0)
        return 0;
    for (a = list; a && n < max; a = a->ifa_next) {
        net_iface *f = &out[n];
        if (!a->ifa_addr || a->ifa_addr->sa_family != AF_INET)
            continue;
        if ((a->ifa_flags & IFF_LOOPBACK) || !(a->ifa_flags & IFF_UP))
            continue;
        memset(f, 0, sizeof(*f));
        snprintf(f->name, sizeof(f->name), "%s", a->ifa_name);
        memcpy(f->ip, &((struct sockaddr_in *)a->ifa_addr)->sin_addr, 4);
        if (a->ifa_netmask)
            memcpy(f->netmask, &((struct sockaddr_in *)a->ifa_netmask)->sin_addr, 4);
        fill_broadcast(f);
        n++;
    }
    freeifaddrs(list);
    return n;
}
#endif

/*
 * udp_socket.h
 * Minimal cross-platform IPv4 UDP socket for Art-Net and sACN
 * (BSD sockets on macOS/Linux, Winsock on Windows). No Max dependency.
 *
 * Addresses are passed as 4 bytes in network order (a.b.c.d -> {a,b,c,d}).
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct udp_socket udp_socket;

/* Opens a socket bound to local_ip (NULL or 0.0.0.0 = any interface) and
 * port (0 = any free port). shared lets other sockets bind the same port,
 * so several objects can listen on 6454. Broadcast sending is enabled.
 * Returns NULL on failure and, if err is given, says why. */
udp_socket *udp_open(const uint8_t *local_ip, uint16_t port, int shared, char *err, size_t errlen);
void udp_close(udp_socket *s);

/* Sends multicast from local_ip's interface (NULL = system default) with TTL ttl. */
int udp_set_multicast(udp_socket *s, const uint8_t *local_ip, int ttl);

/* Returns bytes sent, or -1 (see udp_last_error). */
int udp_sendto(udp_socket *s, const uint8_t ip[4], uint16_t port, const uint8_t *buf, size_t len);

/* Waits up to timeout_ms. Returns bytes read, 0 on timeout, -1 on error.
 * from_ip / from_port may be NULL. */
int udp_recvfrom(udp_socket *s, uint8_t *buf, size_t len, int timeout_ms,
                 uint8_t from_ip[4], uint16_t *from_port);

/* Port the socket is bound to (useful after binding port 0). */
uint16_t udp_local_port(udp_socket *s);

const char *udp_last_error(udp_socket *s);

/* "10.7.157.222" -> {10,7,157,222}. Host names are resolved too (may block).
 * Returns 0 on success. */
int net_parse_ipv4(const char *text, uint8_t ip[4]);
void net_format_ipv4(const uint8_t ip[4], char *out, size_t outlen); /* needs 16 bytes */

typedef struct {
    char    name[64];
    uint8_t ip[4];
    uint8_t netmask[4];
    uint8_t broadcast[4];
} net_iface;

/* Lists up IPv4 interfaces, loopback excluded. Returns the count. */
int net_list_ifaces(net_iface *out, int max);

#ifdef __cplusplus
}
#endif
#endif

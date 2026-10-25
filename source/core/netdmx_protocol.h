/*
 * netdmx_protocol.h
 * Art-Net 4 (ArtDmx, ArtPoll, ArtPollReply) and sACN / ANSI E1.31 packet
 * encoding and decoding. No Max, OS or socket dependency.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef NETDMX_PROTOCOL_H
#define NETDMX_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NETDMX_SLOTS 512

/* ---- Art-Net ----------------------------------------------------------- */

#define ARTNET_PORT              6454
#define ARTNET_PROTOCOL_VERSION  14
#define ARTNET_OP_POLL           0x2000
#define ARTNET_OP_POLL_REPLY     0x2100
#define ARTNET_OP_DMX            0x5000
#define ARTNET_DMX_HEADER_LEN    18
#define ARTNET_DMX_MAX_LEN       (ARTNET_DMX_HEADER_LEN + NETDMX_SLOTS)
#define ARTNET_POLL_LEN          14
#define ARTNET_MAX_PORT_ADDRESS  32767 /* 15 bit: Net (7) | Sub-Net (4) | Universe (4) */

/* Builds an ArtDmx packet for port_address (0..32767). The slot count is
 * rounded up to an even number (2..512) as the spec requires, padding with 0.
 * sequence 0 disables reordering at the receiver; senders normally cycle 1..255.
 * Returns the packet length, or 0 if out is too small. */
size_t artnet_encode_dmx(uint16_t port_address, uint8_t sequence, uint8_t physical,
                         const uint8_t *slots, size_t nslots, uint8_t *out, size_t outlen);

/* Builds an ArtPoll asking every node to reply. Returns the length or 0. */
size_t artnet_encode_poll(uint8_t *out, size_t outlen);

/* Returns the OpCode of an Art-Net packet, or 0 if buf isn't Art-Net. */
uint16_t artnet_opcode(const uint8_t *buf, size_t len);

typedef struct {
    uint8_t  ip[4];
    uint16_t version;           /* firmware version of the node */
    uint16_t oem;
    uint16_t esta;
    char     short_name[18];    /* NUL terminated */
    char     long_name[64];
    char     node_report[64];
    int      num_ports;         /* 0..4 */
    uint8_t  port_types[4];
    uint16_t out_address[4];    /* port-address of each output port */
    uint16_t in_address[4];
    uint8_t  mac[6];
    uint8_t  bind_index;        /* which page of a multi-reply node this is */
} artnet_poll_reply;

/* Decodes an ArtPollReply. Returns 0 on success, -1 if buf isn't one. */
int artnet_decode_poll_reply(const uint8_t *buf, size_t len, artnet_poll_reply *r);

/* Decodes an ArtDmx (used by the tests and by anything listening for it).
 * Returns the number of slots, or -1. */
int artnet_decode_dmx(const uint8_t *buf, size_t len, uint16_t *port_address,
                      uint8_t *sequence, const uint8_t **slots);

/* ---- sACN (E1.31) ------------------------------------------------------ */

#define SACN_PORT             5568
#define SACN_HEADER_LEN       126
#define SACN_MAX_LEN          (SACN_HEADER_LEN + NETDMX_SLOTS)
#define SACN_MIN_UNIVERSE     1
#define SACN_MAX_UNIVERSE     63999
#define SACN_OPT_PREVIEW      0x80
#define SACN_OPT_TERMINATED   0x40
#define SACN_DEFAULT_PRIORITY 100

typedef struct {
    uint8_t  cid[16];          /* sender's UUID, stays the same for the session */
    char     source_name[64];  /* UTF-8, NUL padded on the wire */
    uint8_t  priority;         /* 0..200 */
    uint16_t universe;         /* 1..63999 */
    uint8_t  sequence;
    uint8_t  options;          /* SACN_OPT_* */
    uint8_t  start_code;
} sacn_header;

/* Builds an E1.31 data packet with nslots (0..512) slots after the start
 * code. Returns the packet length, or 0 if out is too small. */
size_t sacn_encode_dmx(const sacn_header *h, const uint8_t *slots, size_t nslots,
                       uint8_t *out, size_t outlen);

/* Decodes an E1.31 data packet. Returns the number of slots, or -1. */
int sacn_decode_dmx(const uint8_t *buf, size_t len, sacn_header *h, const uint8_t **slots);

/* Multicast group of a universe: 239.255.<hi>.<lo>, returned as 4 bytes. */
void sacn_multicast_address(uint16_t universe, uint8_t ip[4]);

/* ---- send pacing ------------------------------------------------------- */

/* Decides when to put a frame on the wire: as soon as something changed, at
 * most rate frames per second, and again every keepalive_ms when nothing
 * changes so receivers that drop a silent sender (and the self-built box,
 * which releases its lock after 5 s) keep the stream. Times are in ms. */
typedef struct {
    double next_allowed;
    double last_sent;
    int    sent_any;
} netdmx_pacer;

void netdmx_pacer_reset(netdmx_pacer *p);
/* Returns 1 if a frame should be sent now; the caller then calls _sent. */
int  netdmx_pacer_due(const netdmx_pacer *p, double now, int dirty, double keepalive_ms);
void netdmx_pacer_sent(netdmx_pacer *p, double now, double rate);

#ifdef __cplusplus
}
#endif
#endif

/*
 * netdmx_protocol.c
 * SPDX-License-Identifier: MIT
 */
#include "netdmx_protocol.h"
#include <string.h>

static const uint8_t artnet_id[8] = { 'A', 'r', 't', '-', 'N', 'e', 't', 0 };
static const uint8_t acn_id[12] = { 'A', 'S', 'C', '-', 'E', '1', '.', '1', '7', 0, 0, 0 };

#define SACN_VECTOR_ROOT_DATA     0x00000004u
#define SACN_VECTOR_FRAMING_DATA  0x00000002u
#define SACN_VECTOR_DMP_SET       0x02

static void put16be(uint8_t *p, uint16_t v) { p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
static void put32be(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v;
}
static uint16_t get16be(const uint8_t *p) { return (uint16_t)((p[0] << 8) | p[1]); }
static uint32_t get32be(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static void copy_name(char *dst, size_t dstlen, const uint8_t *src, size_t srclen)
{
    size_t n = 0;
    while (n < srclen && n + 1 < dstlen && src[n])
        n++;
    memcpy(dst, src, n);
    dst[n] = 0;
}

/* ---- Art-Net ----------------------------------------------------------- */

static void artnet_header(uint8_t *out, uint16_t opcode)
{
    memcpy(out, artnet_id, 8);
    out[8] = (uint8_t)opcode;          /* OpCode is little endian */
    out[9] = (uint8_t)(opcode >> 8);
    out[10] = 0;                       /* ProtVer is big endian */
    out[11] = ARTNET_PROTOCOL_VERSION;
}

size_t artnet_encode_dmx(uint16_t port_address, uint8_t sequence, uint8_t physical,
                         const uint8_t *slots, size_t nslots, uint8_t *out, size_t outlen)
{
    size_t len = nslots;
    if (len > NETDMX_SLOTS)
        len = nslots = NETDMX_SLOTS;
    if (len < 2)
        len = 2;
    len += len & 1;
    if (outlen < ARTNET_DMX_HEADER_LEN + len)
        return 0;
    port_address &= ARTNET_MAX_PORT_ADDRESS;
    artnet_header(out, ARTNET_OP_DMX);
    out[12] = sequence;
    out[13] = physical;
    out[14] = (uint8_t)(port_address & 0xFF);  /* SubUni: Sub-Net << 4 | Universe */
    out[15] = (uint8_t)(port_address >> 8);    /* Net */
    put16be(out + 16, (uint16_t)len);
    if (nslots)
        memcpy(out + ARTNET_DMX_HEADER_LEN, slots, nslots);
    memset(out + ARTNET_DMX_HEADER_LEN + nslots, 0, len - nslots);
    return ARTNET_DMX_HEADER_LEN + len;
}

size_t artnet_encode_poll(uint8_t *out, size_t outlen)
{
    if (outlen < ARTNET_POLL_LEN)
        return 0;
    artnet_header(out, ARTNET_OP_POLL);
    out[12] = 0; /* Flags: reply to this poll only, no diagnostics */
    out[13] = 0; /* DiagPriority */
    return ARTNET_POLL_LEN;
}

uint16_t artnet_opcode(const uint8_t *buf, size_t len)
{
    if (len < 10 || memcmp(buf, artnet_id, 8) != 0)
        return 0;
    return (uint16_t)(buf[8] | (buf[9] << 8));
}

int artnet_decode_poll_reply(const uint8_t *buf, size_t len, artnet_poll_reply *r)
{
    int i;
    /* everything up to and including SwOut; older nodes stop shortly after */
    if (len < 194 || artnet_opcode(buf, len) != ARTNET_OP_POLL_REPLY)
        return -1;
    memset(r, 0, sizeof(*r));
    memcpy(r->ip, buf + 10, 4);
    r->version = get16be(buf + 16);
    r->oem = get16be(buf + 20);
    r->esta = (uint16_t)(buf[24] | (buf[25] << 8));
    copy_name(r->short_name, sizeof(r->short_name), buf + 26, 18);
    copy_name(r->long_name, sizeof(r->long_name), buf + 44, 64);
    copy_name(r->node_report, sizeof(r->node_report), buf + 108, 64);
    r->num_ports = get16be(buf + 172);
    if (r->num_ports > 4)
        r->num_ports = 4;
    if (r->num_ports < 0)
        r->num_ports = 0;
    for (i = 0; i < 4; i++) {
        uint16_t base = (uint16_t)(((buf[18] & 0x7F) << 8) | ((buf[19] & 0x0F) << 4));
        r->port_types[i] = buf[174 + i];
        r->in_address[i] = (uint16_t)(base | (buf[186 + i] & 0x0F));
        r->out_address[i] = (uint16_t)(base | (buf[190 + i] & 0x0F));
    }
    if (len >= 207)
        memcpy(r->mac, buf + 201, 6);
    if (len >= 212)
        r->bind_index = buf[211];
    return 0;
}

int artnet_decode_dmx(const uint8_t *buf, size_t len, uint16_t *port_address,
                      uint8_t *sequence, const uint8_t **slots)
{
    size_t n;
    if (len < ARTNET_DMX_HEADER_LEN || artnet_opcode(buf, len) != ARTNET_OP_DMX)
        return -1;
    n = get16be(buf + 16);
    if (n > NETDMX_SLOTS || ARTNET_DMX_HEADER_LEN + n > len)
        return -1;
    if (port_address)
        *port_address = (uint16_t)(((buf[15] & 0x7F) << 8) | buf[14]);
    if (sequence)
        *sequence = buf[12];
    if (slots)
        *slots = buf + ARTNET_DMX_HEADER_LEN;
    return (int)n;
}

/* ---- sACN (E1.31) ------------------------------------------------------ */

/* PDU flags (0x7) and 12 bit length, length counted from this field */
static void put_flags_len(uint8_t *p, size_t len) { put16be(p, (uint16_t)(0x7000 | (len & 0x0FFF))); }

size_t sacn_encode_dmx(const sacn_header *h, const uint8_t *slots, size_t nslots,
                       uint8_t *out, size_t outlen)
{
    size_t total, n;
    if (nslots > NETDMX_SLOTS)
        nslots = NETDMX_SLOTS;
    total = SACN_HEADER_LEN + nslots;
    if (outlen < total)
        return 0;
    memset(out, 0, SACN_HEADER_LEN);

    /* root layer */
    put16be(out + 0, 0x0010);              /* preamble size */
    put16be(out + 2, 0x0000);              /* postamble size */
    memcpy(out + 4, acn_id, 12);
    put_flags_len(out + 16, total - 16);
    put32be(out + 18, SACN_VECTOR_ROOT_DATA);
    memcpy(out + 22, h->cid, 16);

    /* framing layer */
    put_flags_len(out + 38, total - 38);
    put32be(out + 40, SACN_VECTOR_FRAMING_DATA);
    for (n = 0; n < 63 && h->source_name[n]; n++)
        out[44 + n] = (uint8_t)h->source_name[n];
    out[108] = h->priority > 200 ? 200 : h->priority;
    put16be(out + 109, 0);                 /* synchronization address: none */
    out[111] = h->sequence;
    out[112] = h->options;
    put16be(out + 113, h->universe);

    /* DMP layer */
    put_flags_len(out + 115, total - 115);
    out[117] = SACN_VECTOR_DMP_SET;
    out[118] = 0xA1;                       /* address & data type */
    put16be(out + 119, 0x0000);            /* first property address */
    put16be(out + 121, 0x0001);            /* address increment */
    put16be(out + 123, (uint16_t)(nslots + 1));
    out[125] = h->start_code;
    if (nslots)
        memcpy(out + SACN_HEADER_LEN, slots, nslots);
    return total;
}

int sacn_decode_dmx(const uint8_t *buf, size_t len, sacn_header *h, const uint8_t **slots)
{
    size_t count;
    if (len < SACN_HEADER_LEN || memcmp(buf + 4, acn_id, 12) != 0 ||
        get32be(buf + 18) != SACN_VECTOR_ROOT_DATA || get32be(buf + 40) != SACN_VECTOR_FRAMING_DATA ||
        buf[117] != SACN_VECTOR_DMP_SET)
        return -1;
    count = get16be(buf + 123);
    if (count < 1 || count > NETDMX_SLOTS + 1 || SACN_HEADER_LEN - 1 + count > len)
        return -1;
    if (h) {
        memcpy(h->cid, buf + 22, 16);
        copy_name(h->source_name, sizeof(h->source_name), buf + 44, 64);
        h->priority = buf[108];
        h->sequence = buf[111];
        h->options = buf[112];
        h->universe = get16be(buf + 113);
        h->start_code = buf[125];
    }
    if (slots)
        *slots = buf + SACN_HEADER_LEN;
    return (int)count - 1;
}

void sacn_multicast_address(uint16_t universe, uint8_t ip[4])
{
    ip[0] = 239;
    ip[1] = 255;
    ip[2] = (uint8_t)(universe >> 8);
    ip[3] = (uint8_t)universe;
}

/* ---- send pacing ------------------------------------------------------- */

void netdmx_pacer_reset(netdmx_pacer *p)
{
    memset(p, 0, sizeof(*p));
}

int netdmx_pacer_due(const netdmx_pacer *p, double now, int dirty, double keepalive_ms)
{
    if (now < p->next_allowed)
        return 0;
    if (dirty || !p->sent_any)
        return 1;
    return keepalive_ms > 0 && now - p->last_sent >= keepalive_ms;
}

void netdmx_pacer_sent(netdmx_pacer *p, double now, double rate)
{
    p->last_sent = now;
    p->next_allowed = now + 1000.0 / (rate > 0 ? rate : 40);
    p->sent_any = 1;
}

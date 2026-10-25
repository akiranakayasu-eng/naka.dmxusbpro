/*
 * Unit tests for netdmx_protocol.c and a loopback test of udp_socket.c
 * (no hardware or network needed beyond 127.0.0.1)
 * SPDX-License-Identifier: MIT
 */
#include "netdmx_protocol.h"
#include "udp_socket.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

static void test_artnet_dmx_full(void)
{
    uint8_t slots[512], out[ARTNET_DMX_MAX_LEN];
    const uint8_t *got;
    uint16_t pa = 0;
    uint8_t seq = 0;
    size_t n, i;
    for (i = 0; i < 512; i++)
        slots[i] = (uint8_t)i;
    n = artnet_encode_dmx(0x1234, 7, 0, slots, 512, out, sizeof(out));
    CHECK(n == 18 + 512);
    CHECK(memcmp(out, "Art-Net\0", 8) == 0);
    CHECK(out[8] == 0x00 && out[9] == 0x50); /* OpDmx, little endian */
    CHECK(out[10] == 0 && out[11] == 14);    /* protocol 14, big endian */
    CHECK(out[12] == 7);
    CHECK(out[14] == 0x34);                  /* SubUni */
    CHECK(out[15] == 0x12);                  /* Net */
    CHECK(out[16] == 0x02 && out[17] == 0x00); /* length 512, big endian */
    CHECK(out[18] == 0 && out[18 + 511] == (uint8_t)511);
    CHECK(artnet_decode_dmx(out, n, &pa, &seq, &got) == 512);
    CHECK(pa == 0x1234 && seq == 7 && got[300] == (uint8_t)300);
}

static void test_artnet_dmx_padding(void)
{
    uint8_t slots[3] = { 10, 20, 30 }, out[ARTNET_DMX_MAX_LEN];
    size_t n = artnet_encode_dmx(0, 1, 0, slots, 3, out, sizeof(out));
    CHECK(n == 18 + 4);                      /* rounded up to even */
    CHECK(out[17] == 4);
    CHECK(out[18] == 10 && out[20] == 30 && out[21] == 0);
    n = artnet_encode_dmx(0, 1, 0, slots, 1, out, sizeof(out));
    CHECK(n == 18 + 2);                      /* minimum 2 slots */
    n = artnet_encode_dmx(0x8001, 1, 0, slots, 2, out, sizeof(out));
    CHECK(out[15] == 0x00 && out[14] == 0x01); /* 15 bit port-address */
    CHECK(artnet_encode_dmx(0, 1, 0, slots, 3, out, 20) == 0);
}

static void test_artnet_poll(void)
{
    uint8_t out[32];
    CHECK(artnet_encode_poll(out, sizeof(out)) == 14);
    CHECK(artnet_opcode(out, 14) == ARTNET_OP_POLL);
    CHECK(out[11] == 14);
    CHECK(artnet_opcode((const uint8_t *)"Art-Nyt\0\0\x20", 10) == 0);
}

static void test_artnet_poll_reply(void)
{
    uint8_t p[239];
    artnet_poll_reply r;
    memset(p, 0, sizeof(p));
    memcpy(p, "Art-Net\0", 8);
    p[8] = 0x00; p[9] = 0x21;
    p[10] = 10; p[11] = 7; p[12] = 157; p[13] = 222;
    p[16] = 0x01; p[17] = 0x02;              /* version 0x0102 */
    p[18] = 0x01; p[19] = 0x02;              /* Net 1, Sub-Net 2 */
    p[24] = 0x6C; p[25] = 0x45;              /* ESTA 'El' little endian */
    memcpy(p + 26, "ODE Mk2", 7);
    memcpy(p + 44, "ENTTEC ODE Mk2 two port node", 28);
    memcpy(p + 108, "#0001 [0000] OK", 15);
    p[173] = 2;
    p[174] = 0x80; p[175] = 0x80;
    p[190] = 0x00; p[191] = 0x01;            /* universes 0 and 1 */
    p[201] = 0x00; p[206] = 0x42;
    p[211] = 1;
    CHECK(artnet_decode_poll_reply(p, sizeof(p), &r) == 0);
    CHECK(r.ip[0] == 10 && r.ip[3] == 222);
    CHECK(r.version == 0x0102);
    CHECK(r.esta == 0x456C);
    CHECK(strcmp(r.short_name, "ODE Mk2") == 0);
    CHECK(strcmp(r.long_name, "ENTTEC ODE Mk2 two port node") == 0);
    CHECK(r.num_ports == 2);
    CHECK(r.out_address[0] == 0x120 && r.out_address[1] == 0x121);
    CHECK(r.mac[5] == 0x42 && r.bind_index == 1);

    /* a name filling the whole field still comes back terminated */
    memset(p + 26, 'A', 18);
    CHECK(artnet_decode_poll_reply(p, sizeof(p), &r) == 0);
    CHECK(strlen(r.short_name) == 17);

    CHECK(artnet_decode_poll_reply(p, 100, &r) == -1);
    p[9] = 0x50;
    CHECK(artnet_decode_poll_reply(p, sizeof(p), &r) == -1);
}

static void test_sacn_dmx(void)
{
    sacn_header h, d;
    uint8_t slots[512], out[SACN_MAX_LEN];
    const uint8_t *got;
    size_t n, i;
    memset(&h, 0, sizeof(h));
    for (i = 0; i < 16; i++)
        h.cid[i] = (uint8_t)(0xA0 + i);
    strcpy(h.source_name, "naka.artnet");
    h.priority = 100;
    h.universe = 1;
    h.sequence = 42;
    for (i = 0; i < 512; i++)
        slots[i] = (uint8_t)(255 - i);

    n = sacn_encode_dmx(&h, slots, 512, out, sizeof(out));
    CHECK(n == 638);
    CHECK(out[0] == 0x00 && out[1] == 0x10);
    CHECK(memcmp(out + 4, "ASC-E1.17\0\0\0", 12) == 0);
    CHECK(out[16] == 0x72 && out[17] == 0x6E);   /* 0x7000 | 622 */
    CHECK(out[21] == 0x04);
    CHECK(out[22] == 0xA0 && out[37] == 0xAF);
    CHECK(out[38] == 0x72 && out[39] == 0x58);   /* 0x7000 | 600 */
    CHECK(out[43] == 0x02);
    CHECK(memcmp(out + 44, "naka.artnet", 12) == 0);
    CHECK(out[108] == 100);
    CHECK(out[111] == 42);
    CHECK(out[112] == 0);
    CHECK(out[113] == 0x00 && out[114] == 0x01);
    CHECK(out[115] == 0x72 && out[116] == 0x0B); /* 0x7000 | 523 */
    CHECK(out[117] == 0x02 && out[118] == 0xA1);
    CHECK(out[121] == 0x00 && out[122] == 0x01);
    CHECK(out[123] == 0x02 && out[124] == 0x01); /* 513 */
    CHECK(out[125] == 0x00);
    CHECK(out[126] == 255 && out[637] == (uint8_t)(255 - 511));

    CHECK(sacn_decode_dmx(out, n, &d, &got) == 512);
    CHECK(d.universe == 1 && d.sequence == 42 && d.priority == 100);
    CHECK(strcmp(d.source_name, "naka.artnet") == 0);
    CHECK(got[0] == 255);

    h.options = SACN_OPT_TERMINATED;
    h.priority = 250;                            /* clipped to 200 */
    h.universe = 63999;
    n = sacn_encode_dmx(&h, slots, 24, out, sizeof(out));
    CHECK(n == 126 + 24);
    CHECK(out[112] == 0x40);
    CHECK(out[108] == 200);
    CHECK(out[113] == 0xF9 && out[114] == 0xFF);
    CHECK(sacn_decode_dmx(out, n, NULL, NULL) == 24);
    CHECK(sacn_decode_dmx(out, 100, NULL, NULL) == -1);
    CHECK(sacn_encode_dmx(&h, slots, 512, out, 600) == 0);
}

static void test_sacn_multicast(void)
{
    uint8_t ip[4];
    sacn_multicast_address(1, ip);
    CHECK(ip[0] == 239 && ip[1] == 255 && ip[2] == 0 && ip[3] == 1);
    sacn_multicast_address(0x1234, ip);
    CHECK(ip[2] == 0x12 && ip[3] == 0x34);
}

static void test_pacer(void)
{
    netdmx_pacer p;
    netdmx_pacer_reset(&p);
    CHECK(netdmx_pacer_due(&p, 0, 0, 1000) == 1);  /* first frame always goes */
    netdmx_pacer_sent(&p, 0, 40);
    CHECK(netdmx_pacer_due(&p, 10, 1, 1000) == 0); /* 25 ms between frames at 40 fps */
    CHECK(netdmx_pacer_due(&p, 25, 1, 1000) == 1);
    CHECK(netdmx_pacer_due(&p, 500, 0, 1000) == 0); /* nothing changed */
    CHECK(netdmx_pacer_due(&p, 1000, 0, 1000) == 1); /* keepalive */
    CHECK(netdmx_pacer_due(&p, 5000, 0, 0) == 0);    /* keepalive off */
}

static void test_udp_loopback(void)
{
    uint8_t lo[4] = { 127, 0, 0, 1 }, from[4] = { 0 }, buf[ARTNET_DMX_MAX_LEN], pkt[ARTNET_DMX_MAX_LEN];
    uint8_t slots[4] = { 1, 2, 3, 4 };
    uint16_t from_port = 0, pa = 0;
    const uint8_t *got;
    char err[160] = "";
    udp_socket *rx = udp_open(lo, 0, 0, err, sizeof(err));
    udp_socket *tx = udp_open(lo, 0, 0, err, sizeof(err));
    size_t n;
    int r;
    CHECK(rx && tx);
    if (!rx || !tx) {
        printf("  (%s)\n", err);
        udp_close(rx);
        udp_close(tx);
        return;
    }
    CHECK(udp_recvfrom(rx, buf, sizeof(buf), 10, NULL, NULL) == 0); /* timeout */
    n = artnet_encode_dmx(5, 1, 0, slots, 4, pkt, sizeof(pkt));
    CHECK(udp_sendto(tx, lo, udp_local_port(rx), pkt, n) == (int)n);
    r = udp_recvfrom(rx, buf, sizeof(buf), 500, from, &from_port);
    CHECK(r == (int)n);
    CHECK(from[0] == 127 && from_port == udp_local_port(tx));
    CHECK(artnet_decode_dmx(buf, (size_t)r, &pa, NULL, &got) == 4 && pa == 5 && got[3] == 4);
    udp_close(rx);
    udp_close(tx);
}

static void test_parse_ipv4(void)
{
    uint8_t ip[4];
    char s[16];
    CHECK(net_parse_ipv4("10.7.157.222", ip) == 0);
    CHECK(ip[0] == 10 && ip[1] == 7 && ip[2] == 157 && ip[3] == 222);
    net_format_ipv4(ip, s, sizeof(s));
    CHECK(strcmp(s, "10.7.157.222") == 0);
    CHECK(net_parse_ipv4("", ip) == -1);
    CHECK(net_parse_ipv4("300.1.1.1.invalid", ip) == -1);
}

int main(void)
{
    net_iface ifs[16];
    test_artnet_dmx_full();
    test_artnet_dmx_padding();
    test_artnet_poll();
    test_artnet_poll_reply();
    test_sacn_dmx();
    test_sacn_multicast();
    test_pacer();
    test_udp_loopback();
    test_parse_ipv4();
    printf("%d interface(s) found\n", net_list_ifaces(ifs, 16)); /* must not crash */
    if (failures)
        printf("%d check(s) failed\n", failures);
    else
        printf("all netdmx tests passed\n");
    return failures ? 1 : 0;
}

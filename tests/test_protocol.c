/*
 * Unit tests for dmxusbpro_protocol.c (no hardware needed)
 * SPDX-License-Identifier: MIT
 */
#include "dmxusbpro_protocol.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

static void test_send_dmx_full(void)
{
    uint8_t ch[512], out[DUP_MAX_PACKET_LEN];
    size_t n, i;
    for (i = 0; i < 512; i++)
        ch[i] = (uint8_t)i;
    n = dup_encode_send_dmx(0, ch, 512, out, sizeof(out));
    CHECK(n == 4 + 513 + 1);
    CHECK(out[0] == 0x7E);
    CHECK(out[1] == 6);
    CHECK(out[2] == (513 & 0xFF));
    CHECK(out[3] == (513 >> 8));
    CHECK(out[4] == 0x00);           /* start code */
    CHECK(out[5] == 0);              /* channel 1 */
    CHECK(out[5 + 255] == 255);      /* channel 256 */
    CHECK(out[4 + 512] == (uint8_t)511); /* channel 512 */
    CHECK(out[n - 1] == 0xE7);
}

static void test_send_dmx_min_padding(void)
{
    uint8_t ch[3] = { 10, 20, 30 }, out[DUP_MAX_PACKET_LEN];
    size_t n = dup_encode_send_dmx(0xCC, ch, 3, out, sizeof(out));
    CHECK(n == 4 + 25 + 1);
    CHECK(out[2] == 25 && out[3] == 0);
    CHECK(out[4] == 0xCC); /* custom start code */
    CHECK(out[5] == 10 && out[7] == 30 && out[8] == 0);
    CHECK(out[n - 1] == 0xE7);
}

static void test_encode_too_small(void)
{
    uint8_t out[8];
    CHECK(dup_encode_send_dmx(0, NULL, 0, out, sizeof(out)) == 0);
}

static void test_set_params(void)
{
    uint8_t out[16];
    dup_widget_params p = { 0, 200, 0, 99 };
    size_t n = dup_encode_set_params(&p, out, sizeof(out));
    const uint8_t expect[] = { 0x7E, 4, 5, 0, 0, 0, 127, 1, 40, 0xE7 };
    CHECK(n == sizeof(expect));
    CHECK(memcmp(out, expect, sizeof(expect)) == 0);
}

static void test_get_serial(void)
{
    uint8_t out[8];
    const uint8_t expect[] = { 0x7E, 10, 0, 0, 0xE7 };
    CHECK(dup_encode_get_serial(out, sizeof(out)) == 5);
    CHECK(memcmp(out, expect, 5) == 0);
}

/* ---- decoder ---- */

static int got_count;
static uint8_t got_label;
static uint8_t got_data[600];
static size_t got_len;

static void on_packet(void *ctx, uint8_t label, const uint8_t *data, size_t len)
{
    (void)ctx;
    got_count++;
    got_label = label;
    got_len = len;
    memcpy(got_data, data, len);
}

static void test_decoder_fragmented_with_garbage(void)
{
    dup_decoder d;
    const uint8_t stream[] = {
        0x00, 0x13,                                  /* garbage */
        0x7E, 3, 5, 0, 0x2C, 0x01, 9, 1, 40, 0xE7,   /* params reply fw 1.44 */
        0x7E, 10, 4, 0, 0x23, 0x01, 0x00, 0x20, 0xE7 /* serial 20000123 */
    };
    size_t i;
    dup_widget_params p;
    uint32_t serial = 0;

    dup_decoder_init(&d, on_packet, NULL);
    got_count = 0;
    for (i = 0; i < 12; i++) /* byte by byte for the first packet */
        dup_decoder_feed(&d, stream + i, 1);
    CHECK(got_count == 1);
    CHECK(got_label == 3 && got_len == 5);
    CHECK(dup_parse_params_reply(got_data, got_len, &p) == 0);
    CHECK(p.firmware_version == 0x012C && p.break_time == 9 && p.mab_time == 1 && p.refresh_rate == 40);

    dup_decoder_feed(&d, stream + 12, sizeof(stream) - 12);
    CHECK(got_count == 2);
    CHECK(got_label == 10);
    CHECK(dup_parse_serial_reply(got_data, got_len, &serial) == 0);
    CHECK(serial == 20000123);
}

static void test_decoder_bad_eom_resyncs(void)
{
    dup_decoder d;
    const uint8_t stream[] = {
        0x7E, 3, 1, 0, 0xAA, 0x7E,      /* EOM missing; 0x7E starts next */
        8, 1, 0, 1, 0xE7
    };
    dup_decoder_init(&d, on_packet, NULL);
    got_count = 0;
    dup_decoder_feed(&d, stream, sizeof(stream));
    CHECK(d.errors == 1);
    CHECK(got_count == 1 && got_label == 8 && got_len == 1 && got_data[0] == 1);
}

static void test_received_dmx(void)
{
    const uint8_t data[] = { 0x00, 0x00, 1, 2, 3 };
    uint8_t status, sc;
    const uint8_t *ch;
    size_t n;
    CHECK(dup_parse_received_dmx(data, sizeof(data), &status, &sc, &ch, &n) == 0);
    CHECK(status == 0 && sc == 0 && n == 3 && ch[2] == 3);
}

static void test_cos(void)
{
    uint8_t uni[513];
    /* block 1 => base index 8; bits 0 and 9 => indices 8 and 17
     * (index 0 is the start code, so these are channels 8 and 17) */
    const uint8_t data[] = { 1, 0x01, 0x02, 0, 0, 0, 100, 200 };
    memset(uni, 0, sizeof(uni));
    CHECK(dup_apply_cos(data, sizeof(data), uni) == 0);
    CHECK(uni[8] == 100 && uni[17] == 200 && uni[9] == 0);
}

int main(void)
{
    test_send_dmx_full();
    test_send_dmx_min_padding();
    test_encode_too_small();
    test_set_params();
    test_get_serial();
    test_decoder_fragmented_with_garbage();
    test_decoder_bad_eom_resyncs();
    test_received_dmx();
    test_cos();
    if (failures) {
        printf("%d check(s) failed\n", failures);
        return 1;
    }
    printf("all protocol tests passed\n");
    return 0;
}

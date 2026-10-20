/*
 * dmxusbpro_protocol.c
 * SPDX-License-Identifier: MIT
 */
#include "dmxusbpro_protocol.h"
#include <string.h>

size_t dup_encode(uint8_t label, const uint8_t *data, size_t len,
                  uint8_t *out, size_t out_size)
{
    size_t total = DUP_HEADER_LEN + len + 1;
    if (len > DUP_MAX_DATA_LEN || total > out_size)
        return 0;
    out[0] = DUP_SOM;
    out[1] = label;
    out[2] = (uint8_t)(len & 0xFF);
    out[3] = (uint8_t)((len >> 8) & 0xFF);
    if (len)
        memcpy(out + DUP_HEADER_LEN, data, len);
    out[DUP_HEADER_LEN + len] = DUP_EOM;
    return total;
}

size_t dup_encode_send_dmx(uint8_t start_code, const uint8_t *channels, size_t nchannels,
                           uint8_t *out, size_t out_size)
{
    uint8_t payload[1 + DUP_DMX_CHANNELS];
    size_t n = nchannels;
    if (n > DUP_DMX_CHANNELS)
        n = DUP_DMX_CHANNELS;
    memset(payload, 0, sizeof(payload));
    payload[0] = start_code;
    if (n && channels)
        memcpy(payload + 1, channels, n);
    if (n < DUP_MIN_DMX_CHANNELS)
        n = DUP_MIN_DMX_CHANNELS;
    return dup_encode(DUP_LABEL_OUTPUT_ONLY_SEND_DMX, payload, n + 1, out, out_size);
}

size_t dup_encode_get_params(uint8_t *out, size_t out_size)
{
    const uint8_t data[2] = { 0, 0 }; /* user configuration size = 0 */
    return dup_encode(DUP_LABEL_GET_WIDGET_PARAMS, data, 2, out, out_size);
}

static uint8_t clamp_u8(int v, int lo, int hi)
{
    return (uint8_t)(v < lo ? lo : (v > hi ? hi : v));
}

size_t dup_encode_set_params(const dup_widget_params *p, uint8_t *out, size_t out_size)
{
    uint8_t data[5];
    data[0] = 0; /* user configuration size LSB */
    data[1] = 0; /* user configuration size MSB */
    data[2] = clamp_u8(p->break_time, 9, 127);
    data[3] = clamp_u8(p->mab_time, 1, 127);
    data[4] = clamp_u8(p->refresh_rate, 0, 40);
    return dup_encode(DUP_LABEL_SET_WIDGET_PARAMS, data, 5, out, out_size);
}

size_t dup_encode_get_serial(uint8_t *out, size_t out_size)
{
    return dup_encode(DUP_LABEL_GET_SERIAL_NUMBER, NULL, 0, out, out_size);
}

size_t dup_encode_receive_on_change(int send_on_change, uint8_t *out, size_t out_size)
{
    const uint8_t data[1] = { (uint8_t)(send_on_change ? 1 : 0) };
    return dup_encode(DUP_LABEL_RECEIVE_DMX_ON_CHANGE, data, 1, out, out_size);
}

/* ---- decoder ---------------------------------------------------------- */

enum { S_SOM, S_LABEL, S_LEN_LSB, S_LEN_MSB, S_DATA, S_EOM };

void dup_decoder_init(dup_decoder *d, dup_packet_cb cb, void *ctx)
{
    memset(d, 0, sizeof(*d));
    d->state = S_SOM;
    d->cb = cb;
    d->ctx = ctx;
}

void dup_decoder_feed(dup_decoder *d, const uint8_t *bytes, size_t n)
{
    size_t i;
    for (i = 0; i < n; i++) {
        uint8_t b = bytes[i];
        switch (d->state) {
        case S_SOM:
            if (b == DUP_SOM)
                d->state = S_LABEL;
            break;
        case S_LABEL:
            d->label = b;
            d->state = S_LEN_LSB;
            break;
        case S_LEN_LSB:
            d->len = b;
            d->state = S_LEN_MSB;
            break;
        case S_LEN_MSB:
            d->len |= (uint16_t)(b << 8);
            d->pos = 0;
            if (d->len > DUP_MAX_DATA_LEN) {
                d->errors++;
                d->state = S_SOM;
            } else {
                d->state = d->len ? S_DATA : S_EOM;
            }
            break;
        case S_DATA:
            d->buf[d->pos++] = b;
            if (d->pos >= d->len)
                d->state = S_EOM;
            break;
        case S_EOM:
            if (b == DUP_EOM) {
                if (d->cb)
                    d->cb(d->ctx, d->label, d->buf, d->len);
                d->state = S_SOM;
            } else {
                /* lost sync; resync on the next start-of-message byte */
                d->errors++;
                d->state = (b == DUP_SOM) ? S_LABEL : S_SOM;
            }
            break;
        }
    }
}

/* ---- reply helpers ---------------------------------------------------- */

int dup_parse_params_reply(const uint8_t *data, size_t len, dup_widget_params *p)
{
    if (len < 5)
        return -1;
    p->firmware_version = (uint16_t)(data[0] | (data[1] << 8));
    p->break_time = data[2];
    p->mab_time = data[3];
    p->refresh_rate = data[4];
    return 0;
}

int dup_parse_serial_reply(const uint8_t *data, size_t len, uint32_t *serial)
{
    uint32_t v = 0;
    int i;
    if (len < 4)
        return -1;
    for (i = 3; i >= 0; i--) {
        uint8_t hi = (data[i] >> 4) & 0x0F, lo = data[i] & 0x0F;
        if (hi > 9 || lo > 9)
            return -1;
        v = v * 100 + hi * 10 + lo;
    }
    *serial = v;
    return 0;
}

int dup_parse_received_dmx(const uint8_t *data, size_t len, uint8_t *status,
                           uint8_t *start_code, const uint8_t **channels, size_t *nchannels)
{
    if (len < 2)
        return -1;
    *status = data[0];
    *start_code = data[1];
    *channels = data + 2;
    *nchannels = len - 2;
    return 0;
}

int dup_apply_cos(const uint8_t *data, size_t len, uint8_t *universe)
{
    /* data[0]    : start changed byte number (block of 8 bytes)
     * data[1..5] : 40-bit changed-bit array
     * data[6..]  : changed values, one per set bit, in order
     * Indices count the start code as byte 0. */
    size_t base, src = 6;
    int bit;
    if (len < 6)
        return -1;
    base = (size_t)data[0] * 8;
    for (bit = 0; bit < 40; bit++) {
        if (data[1 + bit / 8] & (1u << (bit % 8))) {
            size_t idx = base + (size_t)bit;
            if (src >= len)
                return -1;
            if (idx <= DUP_DMX_CHANNELS)
                universe[idx] = data[src];
            src++;
        }
    }
    return 0;
}

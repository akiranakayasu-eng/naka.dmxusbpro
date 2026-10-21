/*
 * dmxusbpro_protocol.h
 * ENTTEC DMX USB Pro widget API: packet encoder and streaming decoder.
 * Pure C99, no Max or OS dependencies, so it can be unit tested anywhere.
 *
 * Packet format (all messages, both directions):
 *   0x7E | label | len LSB | len MSB | data[len] | 0xE7
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef DMXUSBPRO_PROTOCOL_H
#define DMXUSBPRO_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUP_SOM 0x7E
#define DUP_EOM 0xE7

#define DUP_DMX_CHANNELS     512
#define DUP_MIN_DMX_CHANNELS 24   /* widget requires >= 24 channels per frame */
#define DUP_MAX_DATA_LEN     600  /* widget API limit for data payload */
#define DUP_HEADER_LEN       4
#define DUP_MAX_PACKET_LEN   (DUP_HEADER_LEN + DUP_MAX_DATA_LEN + 1)

/* Message labels (ENTTEC DMX USB Pro API) */
enum {
    DUP_LABEL_REPROGRAM_FIRMWARE   = 1,
    DUP_LABEL_PROGRAM_FLASH_PAGE   = 2,
    DUP_LABEL_GET_WIDGET_PARAMS    = 3,
    DUP_LABEL_SET_WIDGET_PARAMS    = 4,
    DUP_LABEL_RECEIVED_DMX         = 5,
    DUP_LABEL_OUTPUT_ONLY_SEND_DMX = 6,
    DUP_LABEL_SEND_RDM             = 7,
    DUP_LABEL_RECEIVE_DMX_ON_CHANGE = 8,
    DUP_LABEL_RECEIVED_DMX_COS     = 9,
    DUP_LABEL_GET_SERIAL_NUMBER    = 10,
    DUP_LABEL_SEND_RDM_DISCOVERY   = 11
};

/* Widget timing parameters (label 3 reply / label 4 request) */
typedef struct {
    uint16_t firmware_version; /* reply only */
    uint8_t  break_time;       /* units of 10.67us, valid 9..127 */
    uint8_t  mab_time;         /* units of 10.67us, valid 1..127 */
    uint8_t  refresh_rate;     /* packets/s, 0..40, 0 = as fast as possible */
} dup_widget_params;

/* ---- encoder ---------------------------------------------------------- */

/* Generic packet. Returns bytes written to out, or 0 if it does not fit. */
size_t dup_encode(uint8_t label, const uint8_t *data, size_t len,
                  uint8_t *out, size_t out_size);

/* Label 6. channels[0] is DMX channel 1. nchannels is padded up to 24.
 * start_code is prepended (0x00 = dimmer data). */
size_t dup_encode_send_dmx(uint8_t start_code, const uint8_t *channels, size_t nchannels,
                           uint8_t *out, size_t out_size);

size_t dup_encode_get_params(uint8_t *out, size_t out_size);
size_t dup_encode_set_params(const dup_widget_params *p, uint8_t *out, size_t out_size);
size_t dup_encode_get_serial(uint8_t *out, size_t out_size);
/* Label 8. send_on_change: 0 = widget sends every received frame (label 5),
 * 1 = only changed blocks (label 9). */
size_t dup_encode_receive_on_change(int send_on_change, uint8_t *out, size_t out_size);

/* ---- decoder ---------------------------------------------------------- */

typedef void (*dup_packet_cb)(void *ctx, uint8_t label, const uint8_t *data, size_t len);

typedef struct {
    int      state;
    uint8_t  label;
    uint16_t len;
    uint16_t pos;
    uint8_t  buf[DUP_MAX_DATA_LEN];
    dup_packet_cb cb;
    void    *ctx;
    unsigned long errors;      /* framing errors seen, for diagnostics */
} dup_decoder;

void dup_decoder_init(dup_decoder *d, dup_packet_cb cb, void *ctx);
/* Feed raw bytes from the serial port; cb fires once per complete packet. */
void dup_decoder_feed(dup_decoder *d, const uint8_t *bytes, size_t n);

/* ---- reply helpers ---------------------------------------------------- */

int dup_parse_params_reply(const uint8_t *data, size_t len, dup_widget_params *p);
/* Serial number reply is 4 bytes BCD, little endian. Returns e.g. 2000123. */
int dup_parse_serial_reply(const uint8_t *data, size_t len, uint32_t *serial);
/* Label 5: data[0] = status (bit0 queue overflow, bit1 overrun),
 * data[1] = start code, data[2..] = channels. */
int dup_parse_received_dmx(const uint8_t *data, size_t len, uint8_t *status,
                           uint8_t *start_code, const uint8_t **channels, size_t *nchannels);
/* Label 9: applies a change-of-state block into a 512 byte universe buffer. */
int dup_apply_cos(const uint8_t *data, size_t len, uint8_t *universe /* [513] incl. start code */);

#ifdef __cplusplus
}
#endif
#endif

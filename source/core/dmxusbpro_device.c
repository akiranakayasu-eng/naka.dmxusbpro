/*
 * dmxusbpro_device.c
 * SPDX-License-Identifier: MIT
 */
#include "dmxusbpro_device.h"
#include "dmxusbpro_protocol.h"
#include "serial_port.h"

#define POLL_MS 10

typedef struct {
    int found;
    uint32_t serial;
} probe_ctx;

static void on_packet(void *ctx, uint8_t label, const uint8_t *data, size_t len)
{
    probe_ctx *p = (probe_ctx *)ctx;
    if (label == DUP_LABEL_GET_SERIAL_NUMBER && dup_parse_serial_reply(data, len, &p->serial) == 0)
        p->found = 1;
}

int dup_probe_serial(const char *path, int timeout_ms, uint32_t *serial)
{
    uint8_t pkt[8], rx[256];
    size_t n;
    int waited;
    probe_ctx ctx = { 0, 0 };
    dup_decoder dec;
    serial_port *port = serial_open(path);
    if (!port)
        return -1;

    dup_decoder_init(&dec, on_packet, &ctx);
    n = dup_encode_get_serial(pkt, sizeof(pkt));
    if (serial_write(port, pkt, n) < 0) {
        serial_close(port);
        return -1;
    }
    for (waited = 0; waited < timeout_ms && !ctx.found; waited += POLL_MS) {
        int r = serial_read(port, rx, sizeof(rx), POLL_MS);
        if (r < 0)
            break;
        if (r > 0)
            dup_decoder_feed(&dec, rx, (size_t)r);
    }
    serial_close(port);
    if (!ctx.found)
        return -1;
    *serial = ctx.serial;
    return 0;
}

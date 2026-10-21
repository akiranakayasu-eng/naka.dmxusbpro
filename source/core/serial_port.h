/*
 * serial_port.h
 * Minimal cross-platform serial port for FTDI VCP devices
 * (macOS /dev/cu.usbserial-*, Windows COMx, Linux /dev/ttyUSB*).
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serial_port serial_port;

/* Opens the port in raw 8N1 mode. Baud rate is irrelevant for the
 * DMX USB Pro (USB bulk), 57600 is used for compatibility. */
serial_port *serial_open(const char *path);
void serial_close(serial_port *p);

/* Returns bytes written, or -1 on error (e.g. device unplugged). */
int serial_write(serial_port *p, const unsigned char *buf, size_t len);

/* Waits up to timeout_ms for data. Returns bytes read, 0 on timeout, -1 on error. */
int serial_read(serial_port *p, unsigned char *buf, size_t len, int timeout_ms);

/* Describes the last failed write or read on p (empty if none). */
const char *serial_last_error(serial_port *p);

/* Fills names with candidate ports. On macOS/Linux only USB serial devices
 * are listed. Returns the number of entries written. */
int serial_list(char names[][256], int max);

#ifdef __cplusplus
}
#endif
#endif

/*
 * dmxusbpro_device.h
 * Helpers that talk to a widget over a serial port (no Max dependency).
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef DMXUSBPRO_DEVICE_H
#define DMXUSBPRO_DEVICE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opens path, asks for the widget serial number and closes the port again.
 * Returns 0 and fills *serial on success, -1 if the port can't be opened or
 * nothing answered within timeout_ms (e.g. not a DMX USB Pro, or in use). */
int dup_probe_serial(const char *path, int timeout_ms, uint32_t *serial);

#ifdef __cplusplus
}
#endif
#endif

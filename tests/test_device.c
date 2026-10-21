/*
 * Tests dup_probe_serial against a fake widget on a pseudo terminal.
 * POSIX only; skipped on Windows.
 * SPDX-License-Identifier: MIT
 */
#ifdef _WIN32
#include <stdio.h>
int main(void) { printf("skipped on Windows\n"); return 0; }
#else
#define _XOPEN_SOURCE 600
#define _DEFAULT_SOURCE
#define _DARWIN_C_SOURCE
#include "dmxusbpro_device.h"
#include "dmxusbpro_protocol.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

typedef struct { int fd; int answer; } fake_widget;

/* Replies to a serial number request with 20150128 (BCD, LSB first). */
static void *widget_main(void *arg)
{
    fake_widget *w = (fake_widget *)arg;
    unsigned char buf[64];
    ssize_t n = read(w->fd, buf, sizeof(buf));
    if (w->answer && n >= 5 && buf[0] == 0x7E && buf[1] == DUP_LABEL_GET_SERIAL_NUMBER) {
        const unsigned char reply[] = { 0x7E, 10, 4, 0, 0x28, 0x01, 0x15, 0x20, 0xE7 };
        if (write(w->fd, reply, sizeof(reply)) < 0)
            perror("write");
    }
    return NULL;
}

static int make_pty(char *name, size_t size)
{
    int fd = posix_openpt(O_RDWR | O_NOCTTY);
    struct termios tio;
    if (fd < 0 || grantpt(fd) || unlockpt(fd))
        return -1;
    snprintf(name, size, "%s", ptsname(fd));
    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    tcsetattr(fd, TCSANOW, &tio);
    return fd;
}

static void run(int answer, int expect_rc, uint32_t expect_serial)
{
    char name[128];
    pthread_t t;
    fake_widget w;
    uint32_t serial = 0;
    int rc;
    w.fd = make_pty(name, sizeof(name));
    w.answer = answer;
    CHECK(w.fd >= 0);
    if (w.fd < 0)
        return;
    pthread_create(&t, NULL, widget_main, &w);
    rc = dup_probe_serial(name, 300, &serial);
    CHECK(rc == expect_rc);
    if (expect_rc == 0)
        CHECK(serial == expect_serial);
    close(w.fd); /* unblocks the widget thread if it is still reading */
    pthread_join(t, NULL);
}

int main(void)
{
    uint32_t serial;
    run(1, 0, 20150128);  /* a widget that answers */
    run(0, -1, 0);        /* something that never answers */
    CHECK(dup_probe_serial("/dev/does-not-exist", 50, &serial) == -1);
    if (failures) {
        printf("%d check(s) failed\n", failures);
        return 1;
    }
    printf("all device tests passed\n");
    return 0;
}
#endif

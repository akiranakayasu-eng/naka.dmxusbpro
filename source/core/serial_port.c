/*
 * serial_port.c
 * SPDX-License-Identifier: MIT
 */
#include "serial_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
/* ===================================================================== */
#include <windows.h>

struct serial_port {
    HANDLE h;
    char err[128];
};

static void set_error(serial_port *p, const char *what)
{
    DWORD e = GetLastError();
    char msg[96] = "";
    size_t n;
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, e, 0, msg,
                   sizeof(msg), NULL);
    n = strlen(msg);
    while (n && (msg[n - 1] == '\r' || msg[n - 1] == '\n' || msg[n - 1] == '.'))
        msg[--n] = 0;
    snprintf(p->err, sizeof(p->err), "%s: %s (error %lu)", what, msg, (unsigned long)e);
}

serial_port *serial_open(const char *path)
{
    char full[300];
    HANDLE h;
    DCB dcb;
    COMMTIMEOUTS to;
    serial_port *p;

    /* "\\.\COM10" form is required for COM10 and above */
    if (strncmp(path, "\\\\.\\", 4) == 0)
        snprintf(full, sizeof(full), "%s", path);
    else
        snprintf(full, sizeof(full), "\\\\.\\%s", path);

    h = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;

    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    GetCommState(h, &dcb);
    dcb.BaudRate = 57600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    if (!SetCommState(h, &dcb)) {
        CloseHandle(h);
        return NULL;
    }

    memset(&to, 0, sizeof(to));
    to.ReadIntervalTimeout = MAXDWORD;
    to.ReadTotalTimeoutMultiplier = MAXDWORD;
    to.ReadTotalTimeoutConstant = 10; /* overridden per read */
    to.WriteTotalTimeoutConstant = 500;
    SetCommTimeouts(h, &to);
    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR);

    p = (serial_port *)calloc(1, sizeof(*p));
    p->h = h;
    return p;
}

void serial_close(serial_port *p)
{
    if (!p)
        return;
    CloseHandle(p->h);
    free(p);
}

int serial_write(serial_port *p, const unsigned char *buf, size_t len)
{
    DWORD written = 0;
    if (!WriteFile(p->h, buf, (DWORD)len, &written, NULL)) {
        set_error(p, "write");
        return -1;
    }
    return (int)written;
}

int serial_read(serial_port *p, unsigned char *buf, size_t len, int timeout_ms)
{
    COMMTIMEOUTS to;
    DWORD got = 0;
    GetCommTimeouts(p->h, &to);
    if (to.ReadTotalTimeoutConstant != (DWORD)timeout_ms) {
        to.ReadTotalTimeoutConstant = (DWORD)timeout_ms;
        SetCommTimeouts(p->h, &to);
    }
    if (!ReadFile(p->h, buf, (DWORD)len, &got, NULL)) {
        set_error(p, "read");
        return -1;
    }
    return (int)got;
}

const char *serial_last_error(serial_port *p)
{
    return p ? p->err : "";
}

int serial_list(char names[][256], int max)
{
    int n = 0, i;
    char target[512];
    for (i = 1; i <= 256 && n < max; i++) {
        char name[16];
        snprintf(name, sizeof(name), "COM%d", i);
        if (QueryDosDeviceA(name, target, sizeof(target)))
            snprintf(names[n++], 256, "%s", name);
    }
    return n;
}

#else
/* ===================================================================== */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

struct serial_port {
    int fd;
    char err[128];
};

static void set_error(serial_port *p, const char *what, int e)
{
    snprintf(p->err, sizeof(p->err), "%s: %s (errno %d)", what, strerror(e), e);
}

serial_port *serial_open(const char *path)
{
    struct termios tio;
    serial_port *p;
    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
        return NULL;

    if (tcgetattr(fd, &tio) != 0) {
        close(fd);
        return NULL;
    }
    cfmakeraw(&tio);
    tio.c_cflag |= CLOCAL | CREAD;
    tio.c_cflag &= ~(CSTOPB | PARENB | CRTSCTS);
    tio.c_cflag = (tio.c_cflag & ~CSIZE) | CS8;
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    cfsetispeed(&tio, B57600);
    cfsetospeed(&tio, B57600);
    if (tcsetattr(fd, TCSANOW, &tio) != 0) {
        close(fd);
        return NULL;
    }
    tcflush(fd, TCIOFLUSH);

    /* back to blocking writes; reads use poll() */
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);

    p = (serial_port *)calloc(1, sizeof(*p));
    p->fd = fd;
    return p;
}

void serial_close(serial_port *p)
{
    if (!p)
        return;
    close(p->fd);
    free(p);
}

int serial_write(serial_port *p, const unsigned char *buf, size_t len)
{
    size_t done = 0;
    while (done < len) {
        ssize_t r = write(p->fd, buf + done, len - done);
        if (r < 0) {
            if (errno == EINTR)
                continue;
            set_error(p, "write", errno);
            return -1;
        }
        done += (size_t)r;
    }
    return (int)done;
}

int serial_read(serial_port *p, unsigned char *buf, size_t len, int timeout_ms)
{
    struct pollfd pfd;
    ssize_t r;
    pfd.fd = p->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    r = poll(&pfd, 1, timeout_ms);
    if (r < 0) {
        if (errno == EINTR)
            return 0;
        set_error(p, "poll", errno);
        return -1;
    }
    if (r == 0)
        return 0;
    if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
        snprintf(p->err, sizeof(p->err), "poll: %s%s%s",
                 (pfd.revents & POLLHUP) ? "hangup " : "",
                 (pfd.revents & POLLERR) ? "error " : "",
                 (pfd.revents & POLLNVAL) ? "invalid " : "");
        return -1;
    }
    r = read(p->fd, buf, len);
    if (r < 0) {
        if (errno == EAGAIN || errno == EINTR)
            return 0;
        set_error(p, "read", errno);
        return -1;
    }
    return (int)r;
}

const char *serial_last_error(serial_port *p)
{
    return p ? p->err : "";
}

static int has_prefix(const char *s, const char *prefix)
{
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

int serial_list(char names[][256], int max)
{
    int n = 0;
    struct dirent *e;
    DIR *d = opendir("/dev");
    if (!d)
        return 0;
    while ((e = readdir(d)) != NULL && n < max) {
#ifdef __APPLE__
        /* use callout (cu.) devices; tty. devices block on DCD */
        if (has_prefix(e->d_name, "cu.usbserial"))
#else
        if (has_prefix(e->d_name, "ttyUSB"))
#endif
            snprintf(names[n++], 256, "/dev/%.250s", e->d_name);
    }
    closedir(d);
    return n;
}
#endif

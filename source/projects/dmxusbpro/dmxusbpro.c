/*
 * dmxusbpro — Max external for the ENTTEC DMX USB Pro
 *
 * Message interface is compatible with the discontinued dmxusbpro 1.x
 * object by Olaf Matthes, so existing patches keep working:
 *
 * Creation arguments: [<device> [<channels> [<refresh>]]]
 *   <device> is a port name (/dev/cu.usbserial-XXXX, COM3, or any unique
 *   part of it) or a number (Windows: COM<n>, macOS: n-th port in 'getdevs').
 *
 * Inlet:
 *   list <ch> <val> [<ch> <val> ...]  set channels (1..512); ch 0 sets all
 *   ch <start> v1 v2 ...              set consecutive channels (extension)
 *   open <device> / close
 *   <device> | None                   (from umenu) open that device / close
 *   menu                              fill a umenu from the right outlet
 *   getdevs                           print available devices
 *   blackout <0|1>                    temporarily output all zeros
 *   send / receive                    output (default) or input mode
 *   getparams / getserial / print
 *   breaktime <us> / mabtime <us>     widget DMX timing
 *   setparams <break> <mab> <rate>    raw widget params (extension)
 *   bang                              resend the current frame
 *   firmware                          not supported (prints a notice)
 *
 * Attributes: @channels (1..512, default 512), @refresh (1..44, default 40),
 *             @startcode (0..255, default 0), @baudrate (accepted, unused:
 *             the widget is a USB device so baud rate has no effect)
 *
 * Outlets:
 *   0  received DMX, one list per changed slot: <startcode> <channel> <value>
 *   1  connection state (int 0/1)
 *   2  umenu messages (clear / append ...)
 *
 * The widget repeats the last frame on the DMX line by itself, so frames are
 * sent only when something changed, at most @refresh times per second.
 * All serial I/O runs on one background thread; nothing blocks the scheduler.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ext.h"
#include "ext_obex.h"
#include "ext_systhread.h"
#include "ext_systime.h"

#include "dmxusbpro_protocol.h"
#include "serial_port.h"

#include <math.h>
#include <string.h>

#define MAX_PORTS     64
#define CMD_QUEUE_LEN 1024
#define US_PER_UNIT   10.67

typedef struct _dmxusbpro {
    t_object ob;
    void *out_dmx;
    void *out_state;
    void *out_menu;

    /* shared with the I/O thread, guarded by mutex */
    t_systhread_mutex mutex;
    uint8_t  universe[DUP_DMX_CHANNELS];
    int      dirty;
    int      blackout;
    int      receiving;
    uint8_t  cmdq[CMD_QUEUE_LEN];   /* pre-encoded non-DMX packets */
    size_t   cmdq_len;
    uint8_t  rx_universe[1 + DUP_DMX_CHANNELS]; /* [0] = start code */
    int      rx_dirty;
    dup_widget_params rx_params;
    int      rx_params_ready;
    uint32_t rx_serial;
    int      rx_serial_ready;
    int      io_error;

    /* main thread only */
    uint8_t  rx_reported[1 + DUP_DMX_CHANNELS];
    dup_widget_params params;       /* last known widget timing */
    int      params_quiet;          /* replies to our own query aren't printed */
    char     devname[256];

    /* I/O thread */
    t_systhread thread;
    volatile int running;
    serial_port *port;
    dup_decoder decoder;
    void *qelem;

    /* attributes */
    t_atom_long channels;
    t_atom_long refresh;
    t_atom_long startcode;
    t_atom_long baudrate;

    char port_names[MAX_PORTS][256];
    int  nports;
} t_dmxusbpro;

static t_class *s_class;
static t_symbol *ps_none, *ps_clear, *ps_append;

static void *dmxusbpro_new(t_symbol *s, long argc, t_atom *argv);
static void dmxusbpro_free(t_dmxusbpro *x);
static void dmxusbpro_assist(t_dmxusbpro *x, void *b, long m, long a, char *s);
static void dmxusbpro_list(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv);
static void dmxusbpro_ch(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv);
static void dmxusbpro_blackout(t_dmxusbpro *x, long on);
static void dmxusbpro_bang(t_dmxusbpro *x);
static void dmxusbpro_getdevs(t_dmxusbpro *x);
static void dmxusbpro_menu(t_dmxusbpro *x);
static void dmxusbpro_open(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv);
static void dmxusbpro_anything(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv);
static void dmxusbpro_close(t_dmxusbpro *x);
static void dmxusbpro_send(t_dmxusbpro *x);
static void dmxusbpro_receive(t_dmxusbpro *x);
static void dmxusbpro_getparams(t_dmxusbpro *x);
static void dmxusbpro_getserial(t_dmxusbpro *x);
static void dmxusbpro_breaktime(t_dmxusbpro *x, double us);
static void dmxusbpro_mabtime(t_dmxusbpro *x, double us);
static void dmxusbpro_setparams(t_dmxusbpro *x, long brk, long mab, long rate);
static void dmxusbpro_print(t_dmxusbpro *x);
static void dmxusbpro_firmware(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv);
static t_max_err dmxusbpro_dirty_attr_set(t_dmxusbpro *x, void *attr, long argc, t_atom *argv);
static void dmxusbpro_qfn(t_dmxusbpro *x);
static void *dmxusbpro_threadproc(t_dmxusbpro *x);
static void dmxusbpro_on_packet(void *ctx, uint8_t label, const uint8_t *data, size_t len);

void ext_main(void *r)
{
    t_class *c = class_new("dmxusbpro", (method)dmxusbpro_new, (method)dmxusbpro_free,
                           sizeof(t_dmxusbpro), 0L, A_GIMME, 0);

    class_addmethod(c, (method)dmxusbpro_list,      "list",      A_GIMME, 0);
    class_addmethod(c, (method)dmxusbpro_ch,        "ch",        A_GIMME, 0);
    class_addmethod(c, (method)dmxusbpro_blackout,  "blackout",  A_DEFLONG, 0);
    class_addmethod(c, (method)dmxusbpro_bang,      "bang",      0);
    class_addmethod(c, (method)dmxusbpro_getdevs,   "getdevs",   0);
    class_addmethod(c, (method)dmxusbpro_menu,      "menu",      0);
    class_addmethod(c, (method)dmxusbpro_open,      "open",      A_GIMME, 0);
    class_addmethod(c, (method)dmxusbpro_close,     "close",     0);
    class_addmethod(c, (method)dmxusbpro_send,      "send",      0);
    class_addmethod(c, (method)dmxusbpro_receive,   "receive",   0);
    class_addmethod(c, (method)dmxusbpro_getparams, "getparams", 0);
    class_addmethod(c, (method)dmxusbpro_getserial, "getserial", 0);
    class_addmethod(c, (method)dmxusbpro_breaktime, "breaktime", A_FLOAT, 0);
    class_addmethod(c, (method)dmxusbpro_mabtime,   "mabtime",   A_FLOAT, 0);
    class_addmethod(c, (method)dmxusbpro_setparams, "setparams", A_LONG, A_LONG, A_LONG, 0);
    class_addmethod(c, (method)dmxusbpro_print,     "print",     0);
    class_addmethod(c, (method)dmxusbpro_firmware,  "firmware",  A_GIMME, 0);
    class_addmethod(c, (method)dmxusbpro_anything,  "anything",  A_GIMME, 0);
    class_addmethod(c, (method)dmxusbpro_assist,    "assist",    A_CANT, 0);

    CLASS_ATTR_LONG(c, "channels", 0, t_dmxusbpro, channels);
    CLASS_ATTR_FILTER_CLIP(c, "channels", 1, DUP_DMX_CHANNELS);
    CLASS_ATTR_ACCESSORS(c, "channels", NULL, dmxusbpro_dirty_attr_set);
    CLASS_ATTR_LABEL(c, "channels", 0, "Number of DMX channels to send");

    CLASS_ATTR_LONG(c, "refresh", 0, t_dmxusbpro, refresh);
    CLASS_ATTR_FILTER_CLIP(c, "refresh", 1, 44);
    CLASS_ATTR_LABEL(c, "refresh", 0, "Refresh rate (frames per second)");

    CLASS_ATTR_LONG(c, "startcode", 0, t_dmxusbpro, startcode);
    CLASS_ATTR_FILTER_CLIP(c, "startcode", 0, 255);
    CLASS_ATTR_ACCESSORS(c, "startcode", NULL, dmxusbpro_dirty_attr_set);
    CLASS_ATTR_LABEL(c, "startcode", 0, "DMX start code");

    CLASS_ATTR_LONG(c, "baudrate", 0, t_dmxusbpro, baudrate);
    CLASS_ATTR_LABEL(c, "baudrate", 0, "Baud rate (ignored, kept for compatibility)");

    class_register(CLASS_BOX, c);
    s_class = c;

    ps_none = gensym("None");
    ps_clear = gensym("clear");
    ps_append = gensym("append");
}

/* ---- lifecycle -------------------------------------------------------- */

static void *dmxusbpro_new(t_symbol *s, long argc, t_atom *argv)
{
    t_dmxusbpro *x = (t_dmxusbpro *)object_alloc(s_class);
    long nargs;
    if (!x)
        return NULL;

    x->out_menu = outlet_new(x, NULL);
    x->out_state = intout(x);
    x->out_dmx = listout(x);
    systhread_mutex_new(&x->mutex, 0);
    x->qelem = qelem_new(x, (method)dmxusbpro_qfn);
    x->channels = DUP_DMX_CHANNELS;
    x->refresh = 40;
    x->startcode = 0;
    x->baudrate = 57600;
    x->params.break_time = 9;   /* widget defaults until getparams answers */
    x->params.mab_time = 1;
    x->params.refresh_rate = 40;
    dup_decoder_init(&x->decoder, dmxusbpro_on_packet, x);

    /* <device> [<channels> <refresh>] */
    nargs = attr_args_offset((short)argc, argv);
    if (nargs > 1)
        x->channels = CLAMP(atom_getlong(argv + 1), 1, DUP_DMX_CHANNELS);
    if (nargs > 2)
        x->refresh = CLAMP(atom_getlong(argv + 2), 1, 44);
    attr_args_process(x, (short)argc, argv);

    if (nargs > 0)
        defer_low(x, (method)dmxusbpro_open, NULL, 1, argv);
    return x;
}

static void dmxusbpro_free(t_dmxusbpro *x)
{
    x->out_state = NULL; /* no output while being freed */
    dmxusbpro_close(x);
    qelem_free(x->qelem);
    systhread_mutex_free(x->mutex);
}

static void dmxusbpro_assist(t_dmxusbpro *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s, "(list) <channel> <value> pairs, control messages");
    else if (a == 0)
        sprintf(s, "(list) DMX input: <startcode> <channel> <value>");
    else if (a == 1)
        sprintf(s, "(int) state of connection");
    else
        sprintf(s, "device menu (connect to umenu)");
}

static t_max_err dmxusbpro_dirty_attr_set(t_dmxusbpro *x, void *attr, long argc, t_atom *argv)
{
    t_symbol *name = (t_symbol *)object_method(attr, gensym("getname"));
    if (argc < 1)
        return MAX_ERR_NONE;
    systhread_mutex_lock(x->mutex);
    if (name == gensym("channels"))
        x->channels = CLAMP(atom_getlong(argv), 1, DUP_DMX_CHANNELS);
    else if (name == gensym("startcode"))
        x->startcode = CLAMP(atom_getlong(argv), 0, 255);
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
    return MAX_ERR_NONE;
}

/* ---- DMX values ------------------------------------------------------- */

static uint8_t to_dmx(t_atom *a)
{
    t_atom_long v = (atom_gettype(a) == A_FLOAT) ? (t_atom_long)floor(atom_getfloat(a) + 0.5)
                                                 : atom_getlong(a);
    return (uint8_t)CLAMP(v, 0, 255);
}

static void dmxusbpro_list(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv)
{
    long i;
    if (argc < 2 || (argc & 1)) {
        object_error((t_object *)x, "wrong argument type, need (int)<channel> (int/float)<value> pairs");
        return;
    }
    systhread_mutex_lock(x->mutex);
    for (i = 0; i + 1 < argc; i += 2) {
        t_atom_long ch = atom_getlong(argv + i);
        uint8_t v = to_dmx(argv + i + 1);
        if (ch == 0)
            memset(x->universe, v, sizeof(x->universe));
        else if (ch >= 1 && ch <= DUP_DMX_CHANNELS)
            x->universe[ch - 1] = v;
        else
            object_error((t_object *)x, "channel %ld out of range (1 - 512)", (long)ch);
    }
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

static void dmxusbpro_ch(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv)
{
    long i, start;
    if (argc < 2) {
        object_error((t_object *)x, "ch: expected <channel> <value> [value ...]");
        return;
    }
    start = (long)atom_getlong(argv) - 1;
    systhread_mutex_lock(x->mutex);
    for (i = 1; i < argc; i++) {
        long ch = start + i - 1;
        if (ch >= 0 && ch < DUP_DMX_CHANNELS)
            x->universe[ch] = to_dmx(argv + i);
    }
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

static void dmxusbpro_blackout(t_dmxusbpro *x, long on)
{
    systhread_mutex_lock(x->mutex);
    x->blackout = on ? 1 : 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

static void dmxusbpro_bang(t_dmxusbpro *x)
{
    systhread_mutex_lock(x->mutex);
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

/* ---- widget commands -------------------------------------------------- */

static int require_port(t_dmxusbpro *x, const char *what)
{
    if (x->port)
        return 1;
    object_error((t_object *)x, "can't %s, not connected", what);
    return 0;
}

static void enqueue(t_dmxusbpro *x, const uint8_t *pkt, size_t n)
{
    systhread_mutex_lock(x->mutex);
    if (n && x->cmdq_len + n <= CMD_QUEUE_LEN) {
        memcpy(x->cmdq + x->cmdq_len, pkt, n);
        x->cmdq_len += n;
    }
    systhread_mutex_unlock(x->mutex);
}

static void dmxusbpro_getparams(t_dmxusbpro *x)
{
    uint8_t pkt[16];
    if (require_port(x, "get parameters"))
        enqueue(x, pkt, dup_encode_get_params(pkt, sizeof(pkt)));
}

static void dmxusbpro_getserial(t_dmxusbpro *x)
{
    uint8_t pkt[16];
    if (require_port(x, "get serial number"))
        enqueue(x, pkt, dup_encode_get_serial(pkt, sizeof(pkt)));
}

static void send_params(t_dmxusbpro *x)
{
    uint8_t pkt[16];
    enqueue(x, pkt, dup_encode_set_params(&x->params, pkt, sizeof(pkt)));
}

static void dmxusbpro_breaktime(t_dmxusbpro *x, double us)
{
    if (!require_port(x, "set break time"))
        return;
    x->params.break_time = (uint8_t)CLAMP((long)floor(us / US_PER_UNIT + 0.5), 9, 127);
    send_params(x);
    object_post((t_object *)x, "set break time to %g us", x->params.break_time * US_PER_UNIT);
}

static void dmxusbpro_mabtime(t_dmxusbpro *x, double us)
{
    if (!require_port(x, "set mark after break time"))
        return;
    x->params.mab_time = (uint8_t)CLAMP((long)floor(us / US_PER_UNIT + 0.5), 1, 127);
    send_params(x);
    object_post((t_object *)x, "set mark after break time to %g us", x->params.mab_time * US_PER_UNIT);
}

static void dmxusbpro_setparams(t_dmxusbpro *x, long brk, long mab, long rate)
{
    if (!require_port(x, "set parameters"))
        return;
    x->params.break_time = (uint8_t)CLAMP(brk, 9, 127);
    x->params.mab_time = (uint8_t)CLAMP(mab, 1, 127);
    x->params.refresh_rate = (uint8_t)CLAMP(rate, 0, 40);
    send_params(x);
}

static void dmxusbpro_send(t_dmxusbpro *x)
{
    systhread_mutex_lock(x->mutex);
    x->receiving = 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
    object_post((t_object *)x, "set 'send' mode");
}

static void dmxusbpro_receive(t_dmxusbpro *x)
{
    uint8_t pkt[16];
    systhread_mutex_lock(x->mutex);
    x->receiving = 1;
    systhread_mutex_unlock(x->mutex);
    /* report every slot once after switching, then only changes */
    memset(x->rx_reported, 0xFF, sizeof(x->rx_reported));
    if (x->port)
        enqueue(x, pkt, dup_encode_receive_on_change(1, pkt, sizeof(pkt)));
    object_post((t_object *)x, "set 'receive' mode");
}

static void dmxusbpro_firmware(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv)
{
    object_warn((t_object *)x, "firmware update is not supported, use ENTTEC's own tools");
}

static void dmxusbpro_print(t_dmxusbpro *x)
{
    object_post((t_object *)x, "dmxusbpro (open source), %s",
                x->port ? x->devname : "not connected");
    object_post((t_object *)x, "  mode: %s, channels: %ld, refresh: %ld Hz, startcode: %ld",
                x->receiving ? "receive" : "send", (long)x->channels, (long)x->refresh,
                (long)x->startcode);
    object_post((t_object *)x, "  break time: %g us, mark after break: %g us",
                x->params.break_time * US_PER_UNIT, x->params.mab_time * US_PER_UNIT);
}

/* ---- devices ---------------------------------------------------------- */

static void scan_ports(t_dmxusbpro *x)
{
    x->nports = serial_list(x->port_names, MAX_PORTS);
}

static void dmxusbpro_getdevs(t_dmxusbpro *x)
{
    int i;
    scan_ports(x);
    object_post((t_object *)x, "found %d devices:", x->nports);
    for (i = 0; i < x->nports; i++)
        object_post((t_object *)x, "  %d: %s", i + 1, x->port_names[i]);
}

static void dmxusbpro_menu(t_dmxusbpro *x)
{
    t_atom a;
    int i;
    scan_ports(x);
    outlet_anything(x->out_menu, ps_clear, 0, NULL);
    atom_setsym(&a, ps_none);
    outlet_anything(x->out_menu, ps_append, 1, &a);
    for (i = 0; i < x->nports; i++) {
        atom_setsym(&a, gensym(x->port_names[i]));
        outlet_anything(x->out_menu, ps_append, 1, &a);
    }
}

/* Resolves a device argument to a path. Returns 0 on success. */
static int resolve_port(t_dmxusbpro *x, t_atom *a, char *path, size_t size)
{
    int i;
    scan_ports(x);
    if (atom_gettype(a) == A_LONG || atom_gettype(a) == A_FLOAT) {
        long n = (long)atom_getlong(a);
#ifdef WIN_VERSION
        snprintf(path, size, "COM%ld", n);
        return 0;
#else
        if (n < 1 || n > x->nports)
            return -1;
        snprintf(path, size, "%s", x->port_names[n - 1]);
        return 0;
#endif
    }
    if (atom_gettype(a) == A_SYM) {
        const char *name = atom_getsym(a)->s_name;
        /* exact name first, then a unique part of it (e.g. "EN123456") */
        for (i = 0; i < x->nports; i++)
            if (strcmp(x->port_names[i], name) == 0)
                break;
        if (i == x->nports)
            for (i = 0; i < x->nports; i++)
                if (strstr(x->port_names[i], name))
                    break;
        snprintf(path, size, "%s", i < x->nports ? x->port_names[i] : name);
        return 0;
    }
    return -1;
}

static void set_state(t_dmxusbpro *x, long state)
{
    if (x->out_state)
        outlet_int(x->out_state, state);
}

static void dmxusbpro_open(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv)
{
    char path[256];
    serial_port *p;
    uint8_t pkt[16];

    if (argc < 1 || resolve_port(x, argv, path, sizeof(path)) != 0) {
        object_error((t_object *)x, "'open <device>' needs device name");
        return;
    }
    dmxusbpro_close(x);

    p = serial_open(path);
    if (!p) {
        object_error((t_object *)x, "failed to open device \"%s\"", path);
        set_state(x, 0);
        return;
    }
    x->port = p;
    snprintf(x->devname, sizeof(x->devname), "%s", path);
    dup_decoder_init(&x->decoder, dmxusbpro_on_packet, x);
    systhread_mutex_lock(x->mutex);
    x->io_error = 0;
    x->cmdq_len = 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);

    /* learn the current timing so breaktime/mabtime keep the other values */
    x->params_quiet = 1;
    enqueue(x, pkt, dup_encode_get_params(pkt, sizeof(pkt)));
    if (x->receiving)
        enqueue(x, pkt, dup_encode_receive_on_change(1, pkt, sizeof(pkt)));

    x->running = 1;
    if (systhread_create((method)dmxusbpro_threadproc, x, 0, 0, 0, &x->thread) != 0) {
        object_error((t_object *)x, "failed to start I/O thread");
        x->running = 0;
        x->thread = NULL;
        serial_close(x->port);
        x->port = NULL;
        set_state(x, 0);
        return;
    }
    object_post((t_object *)x, "opened device '%s'", path);
    set_state(x, 1);
}

/* A bare device name (e.g. from a umenu) opens it; None closes. */
static void dmxusbpro_anything(t_dmxusbpro *x, t_symbol *s, long argc, t_atom *argv)
{
    t_atom a;
    int i;
    if (s == ps_none) {
        dmxusbpro_close(x);
        return;
    }
    scan_ports(x);
    for (i = 0; i < x->nports; i++)
        if (strcmp(x->port_names[i], s->s_name) == 0)
            break;
    if (i < x->nports || strncmp(s->s_name, "/dev/", 5) == 0 || strncmp(s->s_name, "COM", 3) == 0) {
        atom_setsym(&a, s);
        dmxusbpro_open(x, NULL, 1, &a);
        return;
    }
    object_error((t_object *)x, "no method for '%s'", s->s_name);
}

static void dmxusbpro_close(t_dmxusbpro *x)
{
    unsigned int ret;
    if (x->thread) {
        x->running = 0;
        systhread_join(x->thread, &ret);
        x->thread = NULL;
    }
    if (x->port) {
        serial_close(x->port);
        x->port = NULL;
        object_post((t_object *)x, "closed device '%s'", x->devname);
        set_state(x, 0);
    }
}

/* ---- I/O thread ------------------------------------------------------- */

static void *dmxusbpro_threadproc(t_dmxusbpro *x)
{
    uint8_t frame[DUP_DMX_CHANNELS];
    uint8_t cmds[CMD_QUEUE_LEN];
    uint8_t pkt[DUP_MAX_PACKET_LEN];
    uint8_t rx[256];
    double next_send = 0;

    while (x->running) {
        double now = systimer_gettime();
        size_t ncmds = 0, nframe = 0;
        uint8_t sc = 0;
        int send = 0, r;

        systhread_mutex_lock(x->mutex);
        if (x->cmdq_len) {
            ncmds = x->cmdq_len;
            memcpy(cmds, x->cmdq, ncmds);
            x->cmdq_len = 0;
        }
        if (x->dirty && !x->receiving && now >= next_send) {
            nframe = (size_t)x->channels;
            if (x->blackout)
                memset(frame, 0, nframe);
            else
                memcpy(frame, x->universe, nframe);
            sc = (uint8_t)x->startcode;
            x->dirty = 0;
            send = 1;
        }
        systhread_mutex_unlock(x->mutex);

        if (ncmds && serial_write(x->port, cmds, ncmds) < 0)
            break;
        if (send) {
            size_t n = dup_encode_send_dmx(sc, frame, nframe, pkt, sizeof(pkt));
            if (serial_write(x->port, pkt, n) < 0)
                break;
            next_send = now + 1000.0 / (double)(x->refresh > 0 ? x->refresh : 40);
        }

        /* the short read timeout also paces the loop */
        r = serial_read(x->port, rx, sizeof(rx), 2);
        if (r < 0)
            break;
        if (r > 0)
            dup_decoder_feed(&x->decoder, rx, (size_t)r);
    }

    if (x->running) { /* left the loop because of an I/O error */
        systhread_mutex_lock(x->mutex);
        x->io_error = 1;
        systhread_mutex_unlock(x->mutex);
        qelem_set(x->qelem);
    }
    systhread_exit(0);
    return NULL;
}

/* runs on the I/O thread */
static void dmxusbpro_on_packet(void *ctx, uint8_t label, const uint8_t *data, size_t len)
{
    t_dmxusbpro *x = (t_dmxusbpro *)ctx;
    systhread_mutex_lock(x->mutex);
    switch (label) {
    case DUP_LABEL_GET_WIDGET_PARAMS:
        if (dup_parse_params_reply(data, len, &x->rx_params) == 0)
            x->rx_params_ready = 1;
        break;
    case DUP_LABEL_GET_SERIAL_NUMBER:
        if (dup_parse_serial_reply(data, len, &x->rx_serial) == 0)
            x->rx_serial_ready = 1;
        break;
    case DUP_LABEL_RECEIVED_DMX: {
        uint8_t status, sc;
        const uint8_t *ch;
        size_t n;
        if (x->receiving && dup_parse_received_dmx(data, len, &status, &sc, &ch, &n) == 0) {
            if (n > DUP_DMX_CHANNELS)
                n = DUP_DMX_CHANNELS;
            x->rx_universe[0] = sc;
            memcpy(x->rx_universe + 1, ch, n);
            x->rx_dirty = 1;
        }
        break;
    }
    case DUP_LABEL_RECEIVED_DMX_COS:
        if (x->receiving && dup_apply_cos(data, len, x->rx_universe) == 0)
            x->rx_dirty = 1;
        break;
    default:
        break;
    }
    systhread_mutex_unlock(x->mutex);
    qelem_set(x->qelem);
}

/* runs on the main thread */
static void dmxusbpro_qfn(t_dmxusbpro *x)
{
    dup_widget_params p;
    uint32_t serial = 0;
    int have_params, have_serial, have_dmx, io_error, i;
    uint8_t uni[1 + DUP_DMX_CHANNELS];

    systhread_mutex_lock(x->mutex);
    have_params = x->rx_params_ready;
    p = x->rx_params;
    have_serial = x->rx_serial_ready;
    serial = x->rx_serial;
    have_dmx = x->rx_dirty && x->receiving;
    if (have_dmx)
        memcpy(uni, x->rx_universe, sizeof(uni));
    io_error = x->io_error;
    x->rx_params_ready = x->rx_serial_ready = x->rx_dirty = x->io_error = 0;
    systhread_mutex_unlock(x->mutex);

    if (io_error) {
        object_error((t_object *)x, "device disconnected or I/O error");
        dmxusbpro_close(x);
    }
    if (have_params) {
        x->params.break_time = p.break_time;
        x->params.mab_time = p.mab_time;
        x->params.refresh_rate = p.refresh_rate;
    }
    if (have_params && x->params_quiet) {
        x->params_quiet = 0;
    } else if (have_params) {
        object_post((t_object *)x, "firmware version: %d.%d", p.firmware_version >> 8,
                    p.firmware_version & 0xFF);
        object_post((t_object *)x, "break time: %g us", p.break_time * US_PER_UNIT);
        object_post((t_object *)x, "mark after break: %g us", p.mab_time * US_PER_UNIT);
        object_post((t_object *)x, "refresh rate: %d Hz", p.refresh_rate);
    }
    if (have_serial)
        object_post((t_object *)x, "serial number: %lu", (unsigned long)serial);
    if (have_dmx) {
        /* one <startcode> <channel> <value> list per changed slot */
        t_atom av[3];
        int sc_changed = uni[0] != x->rx_reported[0];
        for (i = 1; i <= DUP_DMX_CHANNELS; i++) {
            if (!sc_changed && uni[i] == x->rx_reported[i])
                continue;
            atom_setlong(av, uni[0]);
            atom_setlong(av + 1, i);
            atom_setlong(av + 2, uni[i]);
            outlet_list(x->out_dmx, NULL, 3, av);
        }
        memcpy(x->rx_reported, uni, sizeof(uni));
    }
}

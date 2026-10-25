/*
 * naka.artnet — send DMX from Max over Art-Net or sACN (E1.31)
 *
 * One object sends one universe. Values use the same messages as
 * naka.dmxusbpro, so a patch can switch between USB and network output by
 * changing the object name.
 *
 * Creation arguments: [<host> [<universe>]]
 *   <host> is the receiver's IP address (an ENTTEC ODE, the self-built box...).
 *   Without it Art-Net is broadcast and sACN goes to the universe's multicast group.
 *
 * Inlet:
 *   list <ch> <val> [<ch> <val> ...]  set channels (1..512); ch 0 sets all
 *   ch <start> v1 v2 ...              set consecutive channels
 *   blackout <0|1>                    temporarily output all zeros
 *   bang                              resend the current frame now
 *   start / stop                      start / stop sending (see below)
 *   poll [<address>]                  find Art-Net nodes (ArtPoll)
 *   getifaces                         list this computer's network interfaces
 *   print                             print the object state
 *
 * Attributes: @protocol (artnet | sacn), @universe (Art-Net port-address
 *             0..32767 = Net << 8 | Sub-Net << 4 | Universe, sACN 1..63999),
 *             @host (destination IP, empty = broadcast / multicast),
 *             @bind (local IP to send from, empty = any interface),
 *             @channels (1..512), @refresh (1..44 fps),
 *             @keepalive (ms between repeats of an unchanged frame, 0 = off),
 *             @priority (sACN 0..200), @sourcename (sACN source name)
 *
 * Outlets:
 *   0  replies: node <ip> <short name> <long name> <port-address ...>
 *               iface <name> <ip> <netmask> <broadcast>
 *   1  sending state (int 0/1)
 *
 * Sending starts with the first value (or start) and continues until stop.
 * Frames go out when something changed, at most @refresh times per second,
 * and the last frame is repeated every @keepalive ms. Receivers that lock onto
 * one sender (the self-built box) keep the lock only while frames keep coming,
 * so stop is how a patch hands the lights over. For sACN, stop also sends the
 * stream-terminated flag so receivers let go at once.
 * All network I/O runs on one background thread; nothing blocks the scheduler.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ext.h"
#include "ext_obex.h"
#include "ext_systhread.h"
#include "ext_systime.h"

#include "netdmx_protocol.h"
#include "udp_socket.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef NAKA_ARTNET_VERSION
#define NAKA_ARTNET_VERSION "0.1.0"
#endif

#define PROTO_ARTNET   0
#define PROTO_SACN     1
#define MAX_REPLIES    32
#define MAX_IFACES     16
#define SACN_TTL       16
#define TERMINATE_PKTS 3

typedef struct _artnet {
    t_object ob;
    void *out_reply;
    void *out_state;

    /* shared with the I/O thread, guarded by mutex */
    t_systhread_mutex mutex;
    uint8_t  universe[NETDMX_SLOTS];
    int      dirty;
    int      blackout;
    int      active;                  /* frames are going out */
    int      terminate;               /* sACN stream-terminated packets left to send */
    int      proto;
    uint16_t address;                 /* universe or port-address actually used */
    uint8_t  dest[4];
    uint8_t  poll_dest[4];
    int      poll_request;
    artnet_poll_reply replies[MAX_REPLIES];
    int      nreplies;
    int      tx_error;                /* last send failed */
    char     tx_error_msg[160];
    int      tx_error_changed;
    uint8_t  cid[16];

    /* main thread only */
    int      stopped;                 /* user sent stop: values don't restart */
    int      ready;                   /* constructor done, attributes may reopen */
    long     last_state;
    uint8_t  bind_ip[4];
    int      have_bind;
    uint16_t local_port;

    /* I/O thread */
    t_systhread thread;
    volatile int running;
    udp_socket *sock;
    void *qelem;

    /* attributes */
    t_symbol   *protocol;
    t_atom_long universe_attr;
    t_symbol   *host;
    t_symbol   *bind;
    t_atom_long channels;
    t_atom_long refresh;
    t_atom_long keepalive;
    t_atom_long priority;
    t_symbol   *sourcename;
} t_artnet;

static t_class *s_class;
static t_symbol *ps_artnet, *ps_sacn, *ps_empty, *ps_node, *ps_iface;

static void *artnet_new(t_symbol *s, long argc, t_atom *argv);
static void artnet_free(t_artnet *x);
static void artnet_assist(t_artnet *x, void *b, long m, long a, char *s);
static void artnet_list(t_artnet *x, t_symbol *s, long argc, t_atom *argv);
static void artnet_ch(t_artnet *x, t_symbol *s, long argc, t_atom *argv);
static void artnet_blackout(t_artnet *x, long on);
static void artnet_bang(t_artnet *x);
static void artnet_start(t_artnet *x);
static void artnet_stop(t_artnet *x);
static void artnet_poll(t_artnet *x, t_symbol *s, long argc, t_atom *argv);
static void artnet_getifaces(t_artnet *x);
static void artnet_print(t_artnet *x);
static t_max_err artnet_protocol_set(t_artnet *x, void *attr, long argc, t_atom *argv);
static t_max_err artnet_universe_set(t_artnet *x, void *attr, long argc, t_atom *argv);
static t_max_err artnet_host_set(t_artnet *x, void *attr, long argc, t_atom *argv);
static t_max_err artnet_bind_set(t_artnet *x, void *attr, long argc, t_atom *argv);
static t_max_err artnet_locked_attr_set(t_artnet *x, void *attr, long argc, t_atom *argv);
static void artnet_reopen(t_artnet *x);
static void artnet_close(t_artnet *x);
static void artnet_qfn(t_artnet *x);
static void *artnet_threadproc(t_artnet *x);

void ext_main(void *r)
{
    t_class *c = class_new("naka.artnet", (method)artnet_new, (method)artnet_free,
                           sizeof(t_artnet), 0L, A_GIMME, 0);

    class_addmethod(c, (method)artnet_list,      "list",      A_GIMME, 0);
    class_addmethod(c, (method)artnet_ch,        "ch",        A_GIMME, 0);
    class_addmethod(c, (method)artnet_blackout,  "blackout",  A_DEFLONG, 0);
    class_addmethod(c, (method)artnet_bang,      "bang",      0);
    class_addmethod(c, (method)artnet_start,     "start",     0);
    class_addmethod(c, (method)artnet_stop,      "stop",      0);
    class_addmethod(c, (method)artnet_poll,      "poll",      A_GIMME, 0);
    class_addmethod(c, (method)artnet_getifaces, "getifaces", 0);
    class_addmethod(c, (method)artnet_print,     "print",     0);
    class_addmethod(c, (method)artnet_assist,    "assist",    A_CANT, 0);

    CLASS_ATTR_SYM(c, "protocol", 0, t_artnet, protocol);
    CLASS_ATTR_ENUM(c, "protocol", 0, "artnet sacn");
    CLASS_ATTR_ACCESSORS(c, "protocol", NULL, artnet_protocol_set);
    CLASS_ATTR_LABEL(c, "protocol", 0, "Protocol");

    CLASS_ATTR_LONG(c, "universe", 0, t_artnet, universe_attr);
    CLASS_ATTR_ACCESSORS(c, "universe", NULL, artnet_universe_set);
    CLASS_ATTR_LABEL(c, "universe", 0, "Universe (Art-Net port-address or sACN universe)");

    CLASS_ATTR_SYM(c, "host", 0, t_artnet, host);
    CLASS_ATTR_ACCESSORS(c, "host", NULL, artnet_host_set);
    CLASS_ATTR_LABEL(c, "host", 0, "Destination IP (empty = broadcast / multicast)");

    CLASS_ATTR_SYM(c, "bind", 0, t_artnet, bind);
    CLASS_ATTR_ACCESSORS(c, "bind", NULL, artnet_bind_set);
    CLASS_ATTR_LABEL(c, "bind", 0, "Local IP to send from (empty = any)");

    CLASS_ATTR_LONG(c, "channels", 0, t_artnet, channels);
    CLASS_ATTR_FILTER_CLIP(c, "channels", 1, NETDMX_SLOTS);
    CLASS_ATTR_ACCESSORS(c, "channels", NULL, artnet_locked_attr_set);
    CLASS_ATTR_LABEL(c, "channels", 0, "Number of DMX channels to send");

    CLASS_ATTR_LONG(c, "refresh", 0, t_artnet, refresh);
    CLASS_ATTR_FILTER_CLIP(c, "refresh", 1, 44);
    CLASS_ATTR_ACCESSORS(c, "refresh", NULL, artnet_locked_attr_set);
    CLASS_ATTR_LABEL(c, "refresh", 0, "Maximum frames per second");

    CLASS_ATTR_LONG(c, "keepalive", 0, t_artnet, keepalive);
    CLASS_ATTR_FILTER_CLIP(c, "keepalive", 0, 4000);
    CLASS_ATTR_ACCESSORS(c, "keepalive", NULL, artnet_locked_attr_set);
    CLASS_ATTR_LABEL(c, "keepalive", 0, "Repeat an unchanged frame every (ms, 0 = off)");

    CLASS_ATTR_LONG(c, "priority", 0, t_artnet, priority);
    CLASS_ATTR_FILTER_CLIP(c, "priority", 0, 200);
    CLASS_ATTR_ACCESSORS(c, "priority", NULL, artnet_locked_attr_set);
    CLASS_ATTR_LABEL(c, "priority", 0, "sACN priority");

    CLASS_ATTR_SYM(c, "sourcename", 0, t_artnet, sourcename);
    CLASS_ATTR_ACCESSORS(c, "sourcename", NULL, artnet_locked_attr_set);
    CLASS_ATTR_LABEL(c, "sourcename", 0, "sACN source name");

    class_register(CLASS_BOX, c);
    s_class = c;

    post("naka.artnet %s - Art-Net / sACN output for Max, (c) 2026 Akira Nakayasu, MIT", NAKA_ARTNET_VERSION);
    post("  https://github.com/akiranakayasu-eng/naka.dmxusbpro");

    ps_artnet = gensym("artnet");
    ps_sacn = gensym("sacn");
    ps_empty = gensym("");
    ps_node = gensym("node");
    ps_iface = gensym("iface");
}

/* ---- lifecycle -------------------------------------------------------- */

static void make_cid(t_artnet *x)
{
    /* random version 4 UUID; it only has to differ between senders */
    uint32_t seed = (uint32_t)systime_ms() ^ (uint32_t)(uintptr_t)x ^ (uint32_t)rand();
    int i;
    for (i = 0; i < 16; i++) {
        seed = seed * 1664525u + 1013904223u;
        x->cid[i] = (uint8_t)(seed >> 24);
    }
    x->cid[6] = (uint8_t)((x->cid[6] & 0x0F) | 0x40);
    x->cid[8] = (uint8_t)((x->cid[8] & 0x3F) | 0x80);
}

static void *artnet_new(t_symbol *s, long argc, t_atom *argv)
{
    t_artnet *x = (t_artnet *)object_alloc(s_class);
    long nargs;
    if (!x)
        return NULL;

    x->out_state = intout(x);
    x->out_reply = outlet_new(x, NULL);
    systhread_mutex_new(&x->mutex, 0);
    x->qelem = qelem_new(x, (method)artnet_qfn);
    x->protocol = ps_artnet;
    x->host = ps_empty;
    x->bind = ps_empty;
    x->sourcename = gensym("naka.artnet (Max)");
    x->channels = NETDMX_SLOTS;
    x->refresh = 40;
    x->keepalive = 1000;
    x->priority = SACN_DEFAULT_PRIORITY;
    x->last_state = -1;
    make_cid(x);

    /* [<host> [<universe>]] */
    nargs = attr_args_offset((short)argc, argv);
    if (nargs > 0 && atom_gettype(argv) == A_SYM)
        object_attr_setsym(x, gensym("host"), atom_getsym(argv));
    if (nargs > 1)
        object_attr_setlong(x, gensym("universe"), atom_getlong(argv + 1));
    attr_args_process(x, (short)argc, argv);

    x->ready = 1;
    artnet_reopen(x);
    return x;
}

static void artnet_free(t_artnet *x)
{
    x->out_state = NULL; /* no output while being freed */
    artnet_close(x);
    qelem_free(x->qelem);
    systhread_mutex_free(x->mutex);
}

static void artnet_assist(t_artnet *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s, "(list) <channel> <value> pairs, control messages");
    else if (a == 0)
        sprintf(s, "node / iface replies");
    else
        sprintf(s, "(int) sending state");
}

/* ---- destination ------------------------------------------------------ */

static int find_iface(const uint8_t ip[4], net_iface *out)
{
    net_iface ifs[MAX_IFACES];
    int i, n = net_list_ifaces(ifs, MAX_IFACES);
    for (i = 0; i < n; i++) {
        if (memcmp(ifs[i].ip, ip, 4) == 0) {
            *out = ifs[i];
            return 0;
        }
    }
    return -1;
}

/* Broadcast address for Art-Net when no host is set: the bound interface's
 * subnet broadcast, or the limited broadcast. */
static void broadcast_address(t_artnet *x, uint8_t ip[4])
{
    net_iface f;
    if (x->have_bind && find_iface(x->bind_ip, &f) == 0) {
        memcpy(ip, f.broadcast, 4);
        return;
    }
    memset(ip, 255, 4);
}

/* Recomputes protocol, universe and destination for the I/O thread. */
static void update_dest(t_artnet *x)
{
    int proto = x->protocol == ps_sacn ? PROTO_SACN : PROTO_ARTNET;
    t_atom_long u = x->universe_attr;
    uint8_t dest[4];
    int have_host = 0;

    if (proto == PROTO_ARTNET && (u < 0 || u > ARTNET_MAX_PORT_ADDRESS)) {
        object_warn((t_object *)x, "Art-Net universe %ld out of range (0 - 32767)", (long)u);
        u = CLAMP(u, 0, ARTNET_MAX_PORT_ADDRESS);
    } else if (proto == PROTO_SACN && (u < SACN_MIN_UNIVERSE || u > SACN_MAX_UNIVERSE)) {
        object_warn((t_object *)x, "sACN universe %ld out of range (1 - 63999)", (long)u);
        u = CLAMP(u, SACN_MIN_UNIVERSE, SACN_MAX_UNIVERSE);
    }
    if (x->host && x->host != ps_empty) {
        if (net_parse_ipv4(x->host->s_name, dest) == 0)
            have_host = 1;
        else
            object_error((t_object *)x, "can't resolve host '%s'", x->host->s_name);
    }
    if (!have_host) {
        if (proto == PROTO_SACN)
            sacn_multicast_address((uint16_t)u, dest);
        else
            broadcast_address(x, dest);
    }

    systhread_mutex_lock(x->mutex);
    x->proto = proto;
    x->address = (uint16_t)u;
    memcpy(x->dest, dest, 4);
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

static t_max_err artnet_protocol_set(t_artnet *x, void *attr, long argc, t_atom *argv)
{
    t_symbol *p;
    if (argc < 1 || atom_gettype(argv) != A_SYM)
        return MAX_ERR_NONE;
    p = atom_getsym(argv);
    if (p != ps_artnet && p != ps_sacn) {
        object_error((t_object *)x, "unknown protocol '%s' (artnet or sacn)", p->s_name);
        return MAX_ERR_NONE;
    }
    if (p == x->protocol)
        return MAX_ERR_NONE;
    /* sACN has no universe 0; keep the default usable */
    if (p == ps_sacn && x->universe_attr == 0)
        x->universe_attr = 1;
    x->protocol = p;
    if (x->ready)
        artnet_reopen(x);
    return MAX_ERR_NONE;
}

static t_max_err artnet_universe_set(t_artnet *x, void *attr, long argc, t_atom *argv)
{
    if (argc < 1)
        return MAX_ERR_NONE;
    x->universe_attr = atom_getlong(argv);
    if (x->ready)
        update_dest(x);
    return MAX_ERR_NONE;
}

static t_max_err artnet_host_set(t_artnet *x, void *attr, long argc, t_atom *argv)
{
    x->host = (argc > 0 && atom_gettype(argv) == A_SYM) ? atom_getsym(argv) : ps_empty;
    if (x->ready)
        update_dest(x);
    return MAX_ERR_NONE;
}

static t_max_err artnet_bind_set(t_artnet *x, void *attr, long argc, t_atom *argv)
{
    x->bind = (argc > 0 && atom_gettype(argv) == A_SYM) ? atom_getsym(argv) : ps_empty;
    if (x->ready)
        artnet_reopen(x);
    return MAX_ERR_NONE;
}

static t_max_err artnet_locked_attr_set(t_artnet *x, void *attr, long argc, t_atom *argv)
{
    t_symbol *name = (t_symbol *)object_method(attr, gensym("getname"));
    if (argc < 1)
        return MAX_ERR_NONE;
    systhread_mutex_lock(x->mutex);
    if (name == gensym("channels"))
        x->channels = CLAMP(atom_getlong(argv), 1, NETDMX_SLOTS);
    else if (name == gensym("refresh"))
        x->refresh = CLAMP(atom_getlong(argv), 1, 44);
    else if (name == gensym("keepalive"))
        x->keepalive = CLAMP(atom_getlong(argv), 0, 4000);
    else if (name == gensym("priority"))
        x->priority = CLAMP(atom_getlong(argv), 0, 200);
    else if (name == gensym("sourcename") && atom_gettype(argv) == A_SYM)
        x->sourcename = atom_getsym(argv);
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
    return MAX_ERR_NONE;
}

/* ---- socket and thread ------------------------------------------------ */

static void set_state(t_artnet *x)
{
    long state;
    systhread_mutex_lock(x->mutex);
    state = (x->sock && x->active && !x->tx_error) ? 1 : 0;
    systhread_mutex_unlock(x->mutex);
    if (state != x->last_state && x->out_state) {
        x->last_state = state;
        outlet_int(x->out_state, state);
    }
}

static void artnet_close(t_artnet *x)
{
    unsigned int ret;
    if (x->thread) {
        x->running = 0;
        systhread_join(x->thread, &ret);
        x->thread = NULL;
    }
    if (x->sock) {
        udp_close(x->sock);
        x->sock = NULL;
    }
}

/* (Re)opens the socket for the current protocol and bind address. */
static void artnet_reopen(t_artnet *x)
{
    char err[160] = "";
    const uint8_t *local = NULL;
    udp_socket *s;

    artnet_close(x);
    x->have_bind = 0;
    if (x->bind && x->bind != ps_empty) {
        if (net_parse_ipv4(x->bind->s_name, x->bind_ip) != 0) {
            object_error((t_object *)x, "bind: '%s' is not an IP address", x->bind->s_name);
        } else {
            net_iface f;
            x->have_bind = 1;
            local = x->bind_ip;
            if (find_iface(x->bind_ip, &f) != 0)
                object_warn((t_object *)x, "bind: no interface has address %s (see getifaces)",
                            x->bind->s_name);
        }
    }
    update_dest(x);

    if (x->protocol == ps_sacn) {
        s = udp_open(local, 0, 0, err, sizeof(err));
        if (s && udp_set_multicast(s, local, SACN_TTL) != 0)
            object_warn((t_object *)x, "%s", udp_last_error(s));
    } else {
        /* nodes answer ArtPoll on 6454; share it with other Art-Net software */
        s = udp_open(local, ARTNET_PORT, 1, err, sizeof(err));
        if (!s) {
            object_warn((t_object *)x, "port %d is in use (%s), poll replies may not arrive",
                        ARTNET_PORT, err);
            s = udp_open(local, 0, 0, err, sizeof(err));
        }
    }
    if (!s) {
        object_error((t_object *)x, "can't open network socket (%s)", err);
        set_state(x);
        return;
    }
    x->sock = s;
    x->local_port = udp_local_port(s);

    systhread_mutex_lock(x->mutex);
    x->tx_error = 0;
    x->terminate = 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);

    x->running = 1;
    if (systhread_create((method)artnet_threadproc, x, 0, 0, 0, &x->thread) != 0) {
        object_error((t_object *)x, "failed to start network thread");
        x->running = 0;
        x->thread = NULL;
        udp_close(x->sock);
        x->sock = NULL;
    }
    set_state(x);
}

/* ---- DMX values ------------------------------------------------------- */

static uint8_t to_dmx(t_atom *a)
{
    t_atom_long v = (atom_gettype(a) == A_FLOAT) ? (t_atom_long)floor(atom_getfloat(a) + 0.5)
                                                 : atom_getlong(a);
    return (uint8_t)CLAMP(v, 0, 255);
}

/* called with the mutex held: a value starts sending unless the user stopped it */
static void values_changed(t_artnet *x)
{
    x->dirty = 1;
    if (!x->stopped)
        x->active = 1;
}

static void artnet_list(t_artnet *x, t_symbol *s, long argc, t_atom *argv)
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
        else if (ch >= 1 && ch <= NETDMX_SLOTS)
            x->universe[ch - 1] = v;
        else
            object_error((t_object *)x, "channel %ld out of range (1 - 512)", (long)ch);
    }
    values_changed(x);
    systhread_mutex_unlock(x->mutex);
    set_state(x);
}

static void artnet_ch(t_artnet *x, t_symbol *s, long argc, t_atom *argv)
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
        if (ch >= 0 && ch < NETDMX_SLOTS)
            x->universe[ch] = to_dmx(argv + i);
    }
    values_changed(x);
    systhread_mutex_unlock(x->mutex);
    set_state(x);
}

static void artnet_blackout(t_artnet *x, long on)
{
    systhread_mutex_lock(x->mutex);
    x->blackout = on ? 1 : 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
}

static void artnet_bang(t_artnet *x)
{
    systhread_mutex_lock(x->mutex);
    values_changed(x);
    systhread_mutex_unlock(x->mutex);
    set_state(x);
}

static void artnet_start(t_artnet *x)
{
    x->stopped = 0;
    systhread_mutex_lock(x->mutex);
    x->active = 1;
    x->terminate = 0;
    x->dirty = 1;
    systhread_mutex_unlock(x->mutex);
    set_state(x);
}

static void artnet_stop(t_artnet *x)
{
    x->stopped = 1;
    systhread_mutex_lock(x->mutex);
    if (x->active && x->proto == PROTO_SACN)
        x->terminate = TERMINATE_PKTS;
    x->active = 0;
    systhread_mutex_unlock(x->mutex);
    set_state(x);
}

/* ---- discovery and info ----------------------------------------------- */

static void artnet_poll(t_artnet *x, t_symbol *s, long argc, t_atom *argv)
{
    uint8_t ip[4];
    if (x->protocol != ps_artnet) {
        object_error((t_object *)x, "poll works with @protocol artnet only");
        return;
    }
    if (!x->sock) {
        object_error((t_object *)x, "can't poll, no network socket");
        return;
    }
    if (argc > 0 && atom_gettype(argv) == A_SYM) {
        if (net_parse_ipv4(atom_getsym(argv)->s_name, ip) != 0) {
            object_error((t_object *)x, "poll: can't resolve '%s'", atom_getsym(argv)->s_name);
            return;
        }
    } else {
        broadcast_address(x, ip);
    }
    if (x->local_port != ARTNET_PORT)
        object_warn((t_object *)x, "not listening on port %d, nodes may not be found", ARTNET_PORT);
    systhread_mutex_lock(x->mutex);
    memcpy(x->poll_dest, ip, 4);
    x->poll_request = 1;
    systhread_mutex_unlock(x->mutex);
}

static void artnet_getifaces(t_artnet *x)
{
    net_iface ifs[MAX_IFACES];
    int i, n = net_list_ifaces(ifs, MAX_IFACES);
    t_atom av[4];
    char ip[16], mask[16], bc[16];
    if (!n)
        object_post((t_object *)x, "no network interfaces up");
    for (i = 0; i < n; i++) {
        net_format_ipv4(ifs[i].ip, ip, sizeof(ip));
        net_format_ipv4(ifs[i].netmask, mask, sizeof(mask));
        net_format_ipv4(ifs[i].broadcast, bc, sizeof(bc));
        object_post((t_object *)x, "%s: %s netmask %s broadcast %s", ifs[i].name, ip, mask, bc);
        atom_setsym(av, gensym(ifs[i].name));
        atom_setsym(av + 1, gensym(ip));
        atom_setsym(av + 2, gensym(mask));
        atom_setsym(av + 3, gensym(bc));
        outlet_anything(x->out_reply, ps_iface, 4, av);
    }
}

static void artnet_print(t_artnet *x)
{
    char dest[16];
    int active, tx_error;
    systhread_mutex_lock(x->mutex);
    net_format_ipv4(x->dest, dest, sizeof(dest));
    active = x->active;
    tx_error = x->tx_error;
    systhread_mutex_unlock(x->mutex);
    object_post((t_object *)x, "protocol: %s, universe %u", x->protocol->s_name, x->address);
    if (x->protocol == ps_artnet)
        object_post((t_object *)x, "  (Net %u, Sub-Net %u, Universe %u)", x->address >> 8,
                    (x->address >> 4) & 0x0F, x->address & 0x0F);
    object_post((t_object *)x, "destination: %s%s", dest,
                (x->host && x->host != ps_empty) ? "" :
                (x->protocol == ps_sacn ? " (multicast)" : " (broadcast)"));
    object_post((t_object *)x, "bound to: %s, port %u", x->have_bind ? x->bind->s_name : "any interface",
                x->local_port);
    object_post((t_object *)x, "channels: %ld, refresh: %ld fps, keepalive: %ld ms", (long)x->channels,
                (long)x->refresh, (long)x->keepalive);
    object_post((t_object *)x, "state: %s%s", active ? "sending" : (x->stopped ? "stopped" : "waiting for values"),
                tx_error ? " (send failing)" : "");
}

/* ---- I/O thread ------------------------------------------------------- */

static void *artnet_threadproc(t_artnet *x)
{
    uint8_t frame[NETDMX_SLOTS];
    uint8_t pkt[SACN_MAX_LEN > ARTNET_DMX_MAX_LEN ? SACN_MAX_LEN : ARTNET_DMX_MAX_LEN];
    uint8_t rx[600];
    netdmx_pacer pacer;
    sacn_header sh;
    uint8_t seq = 0, dest[4] = { 0 }, poll_dest[4], from[4];
    int was_active = 0, proto = PROTO_ARTNET;
    size_t nframe = NETDMX_SLOTS;
    uint16_t address = 0;

    netdmx_pacer_reset(&pacer);
    memset(&sh, 0, sizeof(sh));
    memset(frame, 0, sizeof(frame));

    while (x->running) {
        double now = systimer_gettime();
        int send = 0, poll = 0, terminate = 0, last_terminate = 0, r;
        size_t n = 0;

        systhread_mutex_lock(x->mutex);
        proto = x->proto;
        if (x->active && netdmx_pacer_due(&pacer, now, x->dirty, (double)x->keepalive)) {
            nframe = (size_t)x->channels;
            if (x->blackout)
                memset(frame, 0, nframe);
            else
                memcpy(frame, x->universe, nframe);
            address = x->address;
            memcpy(dest, x->dest, 4);
            sh.priority = (uint8_t)x->priority;
            snprintf(sh.source_name, sizeof(sh.source_name), "%s", x->sourcename->s_name);
            x->dirty = 0;
            send = 1;
        } else if (!x->active && x->terminate > 0) {
            x->terminate--;
            terminate = was_active; /* only after something was sent */
            last_terminate = x->terminate == 0;
        }
        if (!x->active)
            netdmx_pacer_reset(&pacer);
        if (x->poll_request) {
            x->poll_request = 0;
            memcpy(poll_dest, x->poll_dest, 4);
            poll = 1;
        }
        memcpy(sh.cid, x->cid, 16);
        systhread_mutex_unlock(x->mutex);

        if (send || terminate) {
            if (proto == PROTO_SACN) {
                sh.universe = address;
                sh.sequence = seq++;
                sh.options = terminate ? SACN_OPT_TERMINATED : 0;
                n = sacn_encode_dmx(&sh, frame, nframe, pkt, sizeof(pkt));
            } else if (send) {
                seq = (uint8_t)(seq % 255 + 1); /* 1..255, 0 would disable ordering */
                n = artnet_encode_dmx(address, seq, 0, frame, nframe, pkt, sizeof(pkt));
            }
            if (n) {
                int ok = udp_sendto(x->sock, dest, proto == PROTO_SACN ? SACN_PORT : ARTNET_PORT, pkt, n) >= 0;
                systhread_mutex_lock(x->mutex);
                if (ok == x->tx_error) { /* state flipped: report on the main thread */
                    x->tx_error = !ok;
                    snprintf(x->tx_error_msg, sizeof(x->tx_error_msg), "%s", udp_last_error(x->sock));
                    x->tx_error_changed = 1;
                    qelem_set(x->qelem);
                }
                systhread_mutex_unlock(x->mutex);
            }
            if (send)
                netdmx_pacer_sent(&pacer, now, (double)x->refresh);
            if (send)
                was_active = 1;
            else if (last_terminate)
                was_active = 0;
        }
        if (poll) {
            n = artnet_encode_poll(pkt, sizeof(pkt));
            udp_sendto(x->sock, poll_dest, ARTNET_PORT, pkt, n);
        }

        /* the short receive timeout also paces the loop */
        r = udp_recvfrom(x->sock, rx, sizeof(rx), 2, from, NULL);
        if (r > 0 && artnet_opcode(rx, (size_t)r) == ARTNET_OP_POLL_REPLY) {
            artnet_poll_reply reply;
            if (artnet_decode_poll_reply(rx, (size_t)r, &reply) == 0) {
                systhread_mutex_lock(x->mutex);
                if (x->nreplies < MAX_REPLIES)
                    x->replies[x->nreplies++] = reply;
                systhread_mutex_unlock(x->mutex);
                qelem_set(x->qelem);
            }
        }
    }

    /* closing while sending sACN: tell receivers the stream ended */
    if (proto == PROTO_SACN && was_active) {
        int i;
        sh.options = SACN_OPT_TERMINATED;
        for (i = 0; i < TERMINATE_PKTS; i++) {
            size_t n;
            sh.sequence = seq++;
            n = sacn_encode_dmx(&sh, frame, nframe, pkt, sizeof(pkt));
            udp_sendto(x->sock, dest, SACN_PORT, pkt, n);
        }
    }
    systhread_exit(0);
    return NULL;
}

/* runs on the main thread */
static void artnet_qfn(t_artnet *x)
{
    artnet_poll_reply replies[MAX_REPLIES];
    int nreplies, i, j, changed, tx_error;
    char msg[160];

    systhread_mutex_lock(x->mutex);
    nreplies = x->nreplies;
    memcpy(replies, x->replies, sizeof(replies[0]) * (size_t)nreplies);
    x->nreplies = 0;
    changed = x->tx_error_changed;
    tx_error = x->tx_error;
    snprintf(msg, sizeof(msg), "%s", x->tx_error_msg);
    x->tx_error_changed = 0;
    systhread_mutex_unlock(x->mutex);

    if (changed && tx_error)
        object_error((t_object *)x, "can't send (%s), retrying", msg[0] ? msg : "unknown error");
    else if (changed)
        object_post((t_object *)x, "sending again");
    if (changed)
        set_state(x);

    for (i = 0; i < nreplies; i++) {
        artnet_poll_reply *r = &replies[i];
        t_atom av[3 + 4];
        char ip[16];
        int ac = 3;
        net_format_ipv4(r->ip, ip, sizeof(ip));
        atom_setsym(av, gensym(ip));
        atom_setsym(av + 1, gensym(r->short_name));
        atom_setsym(av + 2, gensym(r->long_name));
        for (j = 0; j < r->num_ports; j++)
            if (r->port_types[j] & 0x80) /* port can output DMX */
                atom_setlong(av + ac++, r->out_address[j]);
        object_post((t_object *)x, "node %s \"%s\" (%s)%s%s", ip, r->short_name, r->long_name,
                    r->node_report[0] ? ": " : "", r->node_report);
        outlet_anything(x->out_reply, ps_node, ac, av);
    }
}

/*
 * naka.miseru — "brightness, colour, warmth of white" for DMX lights, the miseru way
 *
 * Turns a light's look (brightness, colour strength, hue, white warmth) into DMX channel
 * values, using the same fixture definitions and maths as the miseru app (miseru-lighting).
 * It only converts: send its output to naka.artnet or naka.dmxusbpro to reach the lights.
 *
 * Messages:
 *   patch <name> <type> <address> [<group>]  add a light (address 1..512)
 *   studio                         load the TMU photo studio patch (arri1-3, diva1-4)
 *   clear                          remove all lights
 *   look <target> <intensity> <sat> <hue> <cct>
 *                                  set a light's whole look. <target> is a light name,
 *                                  a group name or "all". intensity and sat are 0..1,
 *                                  hue is 0..360, cct is in kelvin
 *   intensity|sat|hue|cct <target> <value>   change one part of the look
 *   master <0..1>                  overall brightness
 *   blackout <0|1>                 all lights dark (their looks are kept)
 *   allon                          switch on the lights that are off, at full brightness
 *   reset                          every light back to off, white, 5600 K
 *   bang                           output the frame again
 *   types                          print the fixture types
 *   print                          print the patch
 *
 * Types: dimmer, rgb, rgbw, dim-rgb, dim-rgbw, arri-l7c, diva-lite
 *
 * Outlet: "ch 1 <v1> <v2> ..."  the frame up to the last patched channel, in the form
 *         naka.artnet and naka.dmxusbpro accept. It is sent whenever a message changed
 *         the frame, and again on bang.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ext.h"
#include "ext_obex.h"

#include "miseru_render.h"

#include <stdio.h>
#include <string.h>

#ifndef NAKA_MISERU_VERSION
#define NAKA_MISERU_VERSION "0.1.0"
#endif

typedef struct _miseru {
    t_object ob;
    void *out;
    miseru_show show;
} t_miseru;

static t_class *s_class;

static void *miseru_new(t_symbol *s, long argc, t_atom *argv);
static void miseru_assist(t_miseru *x, void *b, long m, long a, char *s);
static void miseru_patch(t_miseru *x, t_symbol *s, long argc, t_atom *argv);
static void miseru_studio(t_miseru *x);
static void miseru_clear(t_miseru *x);
static void miseru_look(t_miseru *x, t_symbol *s, long argc, t_atom *argv);
static void miseru_part(t_miseru *x, t_symbol *s, long argc, t_atom *argv);
static void miseru_master(t_miseru *x, double v);
static void miseru_blackout(t_miseru *x, long on);
static void miseru_allon(t_miseru *x);
static void miseru_reset(t_miseru *x);
static void miseru_bang(t_miseru *x);
static void miseru_types(t_miseru *x);
static void miseru_print(t_miseru *x);

void ext_main(void *r)
{
    t_class *c = class_new("naka.miseru", (method)miseru_new, NULL, sizeof(t_miseru), 0L, A_GIMME, 0);

    class_addmethod(c, (method)miseru_patch,     "patch",     A_GIMME, 0);
    class_addmethod(c, (method)miseru_studio,    "studio",    0);
    class_addmethod(c, (method)miseru_clear,     "clear",     0);
    class_addmethod(c, (method)miseru_look,      "look",      A_GIMME, 0);
    class_addmethod(c, (method)miseru_part,      "intensity", A_GIMME, 0);
    class_addmethod(c, (method)miseru_part,      "sat",       A_GIMME, 0);
    class_addmethod(c, (method)miseru_part,      "hue",       A_GIMME, 0);
    class_addmethod(c, (method)miseru_part,      "cct",       A_GIMME, 0);
    class_addmethod(c, (method)miseru_master,    "master",    A_FLOAT, 0);
    class_addmethod(c, (method)miseru_blackout,  "blackout",  A_DEFLONG, 0);
    class_addmethod(c, (method)miseru_allon,     "allon",     0);
    class_addmethod(c, (method)miseru_reset,     "reset",     0);
    class_addmethod(c, (method)miseru_bang,      "bang",      0);
    class_addmethod(c, (method)miseru_types,     "types",     0);
    class_addmethod(c, (method)miseru_print,     "print",     0);
    class_addmethod(c, (method)miseru_assist,    "assist",    A_CANT, 0);

    class_register(CLASS_BOX, c);
    s_class = c;

    post("naka.miseru %s - miseru colour engine for Max, (c) 2026 Akira Nakayasu, MIT", NAKA_MISERU_VERSION);
}

static void *miseru_new(t_symbol *s, long argc, t_atom *argv)
{
    t_miseru *x = (t_miseru *)object_alloc(s_class);
    if (!x)
        return NULL;
    x->out = outlet_new(x, NULL);
    miseru_show_init(&x->show);
    return x;
}

static void miseru_assist(t_miseru *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s, "patch, look <target> ..., master, blackout, allon, reset");
    else
        sprintf(s, "ch 1 <values...> (to naka.artnet / naka.dmxusbpro)");
}

/* ---- output ----------------------------------------------------------- */

static int last_channel(const miseru_show *s)
{
    int i, last = 0;
    for (i = 0; i < s->count; i++) {
        int end = s->lights[i].address + s->lights[i].type->count - 1;
        if (end > last)
            last = end;
    }
    return last;
}

static void emit(t_miseru *x)
{
    uint8_t frame[MISERU_SLOTS];
    t_atom out[MISERU_SLOTS + 1];
    int n = last_channel(&x->show), i;
    if (n == 0)
        return;
    miseru_show_render(&x->show, frame);
    atom_setlong(out, 1);
    for (i = 0; i < n; i++)
        atom_setlong(out + 1 + i, frame[i]);
    outlet_anything(x->out, gensym("ch"), n + 1, out);
}

static void miseru_bang(t_miseru *x) { emit(x); }

/* ---- patch ------------------------------------------------------------ */

static void miseru_patch(t_miseru *x, t_symbol *s, long argc, t_atom *argv)
{
    const char *group = "";
    int r;
    if (argc < 3 || atom_gettype(argv) != A_SYM || atom_gettype(argv + 1) != A_SYM) {
        object_error((t_object *)x, "patch <name> <type> <address> [<group>]");
        return;
    }
    if (argc >= 4 && atom_gettype(argv + 3) == A_SYM)
        group = atom_getsym(argv + 3)->s_name;
    r = miseru_show_add(&x->show, atom_getsym(argv)->s_name, group, atom_getsym(argv + 1)->s_name, (int)atom_getlong(argv + 2));
    switch (r) {
    case 0: emit(x); break;
    case -1: object_error((t_object *)x, "'%s': name already used or too many lights", atom_getsym(argv)->s_name); break;
    case -2: object_error((t_object *)x, "unknown type '%s' (send 'types' to list them)", atom_getsym(argv + 1)->s_name); break;
    case -3: object_error((t_object *)x, "address %ld does not fit in 1..512", atom_getlong(argv + 2)); break;
    default: object_error((t_object *)x, "'%s' overlaps another light's channels", atom_getsym(argv)->s_name); break;
    }
}

static void miseru_studio(t_miseru *x)
{
    miseru_show_studio(&x->show);
    emit(x);
}

static void miseru_clear(t_miseru *x)
{
    miseru_show_init(&x->show);
}

/* ---- looks ------------------------------------------------------------ */

static double clamp(double v, double lo, double hi) { return v < lo ? lo : v > hi ? hi : v; }

static int target_lights(t_miseru *x, t_atom *a, int *hit)
{
    int i, n = 0;
    const char *target;
    if (atom_gettype(a) != A_SYM) {
        object_error((t_object *)x, "first argument is a light name, a group name or all");
        return -1;
    }
    target = atom_getsym(a)->s_name;
    for (i = 0; i < x->show.count; i++)
        if (miseru_show_matches(&x->show, i, target))
            hit[n++] = i;
    if (n == 0) {
        object_error((t_object *)x, "no light or group called '%s'", target);
        return -1;
    }
    return n;
}

static void miseru_look(t_miseru *x, t_symbol *s, long argc, t_atom *argv)
{
    int hit[MISERU_MAX_LIGHTS], n, i;
    if (argc < 5) {
        object_error((t_object *)x, "look <target> <intensity> <sat> <hue> <cct>");
        return;
    }
    n = target_lights(x, argv, hit);
    if (n < 0)
        return;
    for (i = 0; i < n; i++) {
        miseru_look *l = &x->show.lights[hit[i]].look;
        l->intensity = clamp(atom_getfloat(argv + 1), 0, 1);
        l->sat = clamp(atom_getfloat(argv + 2), 0, 1);
        l->hue = atom_getfloat(argv + 3);
        l->cct = clamp(atom_getfloat(argv + 4), 1000, 40000);
    }
    emit(x);
}

/* intensity / sat / hue / cct <target> <value> */
static void miseru_part(t_miseru *x, t_symbol *s, long argc, t_atom *argv)
{
    int hit[MISERU_MAX_LIGHTS], n, i;
    double v;
    if (argc < 2) {
        object_error((t_object *)x, "%s <target> <value>", s->s_name);
        return;
    }
    n = target_lights(x, argv, hit);
    if (n < 0)
        return;
    v = atom_getfloat(argv + 1);
    for (i = 0; i < n; i++) {
        miseru_look *l = &x->show.lights[hit[i]].look;
        if (strcmp(s->s_name, "intensity") == 0) l->intensity = clamp(v, 0, 1);
        else if (strcmp(s->s_name, "sat") == 0)  l->sat = clamp(v, 0, 1);
        else if (strcmp(s->s_name, "hue") == 0)  l->hue = v;
        else                                     l->cct = clamp(v, 1000, 40000);
    }
    emit(x);
}

static void miseru_master(t_miseru *x, double v)
{
    x->show.master = clamp(v, 0, 1);
    emit(x);
}

static void miseru_blackout(t_miseru *x, long on)
{
    x->show.blackout = on != 0;
    emit(x);
}

static void miseru_allon(t_miseru *x)
{
    miseru_show_all_on(&x->show);
    emit(x);
}

static void miseru_reset(t_miseru *x)
{
    miseru_show_reset(&x->show);
    emit(x);
}

/* ---- info ------------------------------------------------------------- */

static void miseru_types(t_miseru *x)
{
    size_t i;
    for (i = 0; i < miseru_type_count(); i++)
        post("naka.miseru: %s (%d ch)", miseru_type_at(i)->id, miseru_type_at(i)->count);
}

static void miseru_print(t_miseru *x)
{
    int i;
    post("naka.miseru: %d light(s), master %.2f%s", x->show.count, x->show.master, x->show.blackout ? ", blackout" : "");
    for (i = 0; i < x->show.count; i++) {
        const miseru_light *l = &x->show.lights[i];
        post("  %s [%s] %s ch %d-%d  intensity %.2f sat %.2f hue %.0f cct %.0f", l->name, l->group, l->type->id,
             l->address, l->address + l->type->count - 1, l->look.intensity, l->look.sat, l->look.hue, l->look.cct);
    }
}

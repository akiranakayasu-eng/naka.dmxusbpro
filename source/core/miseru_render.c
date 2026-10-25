/* SPDX-License-Identifier: MIT */
#include "miseru_render.h"

#include <math.h>
#include <string.h>

const miseru_look miseru_default_look = { 0.0, 0.0, 0.0, 5600.0 };

#define R(k)       { MISERU_##k, 0, 0 }
#define RA(k, a_)  { MISERU_##k, (a_), 0 }
#define RAB(k, a_, b_) { MISERU_##k, (a_), (b_) }

static const miseru_fixture_type types[] = {
    { "dimmer", 1, { R(DIMMER) } },
    { "rgb", 3, { R(RED), R(GREEN), R(BLUE) } },
    { "rgbw", 4, { R(RED), R(GREEN), R(BLUE), R(WHITE) } },
    { "dim-rgb", 4, { R(DIMMER), R(RED), R(GREEN), R(BLUE) } },
    { "dim-rgbw", 5, { R(DIMMER), R(RED), R(GREEN), R(BLUE), R(WHITE) } },
    /* ARRI L7-C Plus, Mode 1 CCT & RGBW 8 bit (12ch) */
    { "arri-l7c", 12, { R(DIMMER), RAB(CCT, 2800, 10000), RA(TINT, 128), R(XFADE), R(RED), R(GREEN),
                        R(BLUE), R(WHITE), RA(FIXED, 0), RA(FIXED, 0), RA(FIXED, 0), RA(FIXED, 0) } },
    /* Kino Flo Diva-Lite 31 P25 (8ch). The channel order is provisional, as in miseru-lighting. */
    { "diva-lite", 8, { R(DIMMER), RAB(CCT, 2500, 9900), RA(TINT, 128), R(XFADE), R(RED), R(GREEN),
                        R(BLUE), RA(FIXED, 0) } },
};

double miseru_clamp01(double v)
{
    return v < 0 ? 0 : v > 1 ? 1 : v;
}

static uint8_t to8(double v)
{
    return (uint8_t)floor(miseru_clamp01(v) * 255.0 + 0.5);
}

const miseru_fixture_type *miseru_find_type(const char *id)
{
    size_t i;
    for (i = 0; i < sizeof(types) / sizeof(types[0]); i++)
        if (strcmp(types[i].id, id) == 0)
            return &types[i];
    return NULL;
}

size_t miseru_type_count(void) { return sizeof(types) / sizeof(types[0]); }
const miseru_fixture_type *miseru_type_at(size_t i) { return i < miseru_type_count() ? &types[i] : NULL; }

static void hue_to_rgb(double hue, double rgb[3])
{
    double h = fmod(fmod(hue, 360.0) + 360.0, 360.0) / 60.0;
    double x = 1.0 - fabs(fmod(h, 2.0) - 1.0);
    /* per segment, [r,g,b] as 0 = zero, 1 = full, 2 = x: (1,x,0) (x,1,0) (0,1,x) (0,x,1) (x,0,1) (1,0,x) */
    static const int kind[6][3] = { { 1, 2, 0 }, { 2, 1, 0 }, { 0, 1, 2 }, { 0, 2, 1 }, { 2, 0, 1 }, { 1, 0, 2 } };
    int seg = (int)floor(h) % 6, i;
    for (i = 0; i < 3; i++)
        rgb[i] = kind[seg][i] == 0 ? 0.0 : kind[seg][i] == 1 ? 1.0 : x;
}

/* Tanner Helland approximation, scaled so the strongest channel is 1 */
static void cct_to_rgb(double kelvin, double rgb[3])
{
    double t = fmin(40000.0, fmax(1000.0, kelvin)) / 100.0, r, g, b, m;
    if (t <= 66) {
        r = 255;
        g = 99.4708025861 * log(t) - 161.1195681661;
        b = t <= 19 ? 0 : 138.5177312231 * log(t - 10) - 305.0447927307;
    } else {
        r = 329.698727446 * pow(t - 60, -0.1332047592);
        g = 288.1221695283 * pow(t - 60, -0.0755148492);
        b = 255;
    }
    rgb[0] = miseru_clamp01(r / 255);
    rgb[1] = miseru_clamp01(g / 255);
    rgb[2] = miseru_clamp01(b / 255);
    m = fmax(rgb[0], fmax(rgb[1], rgb[2]));
    rgb[0] /= m;
    rgb[1] /= m;
    rgb[2] /= m;
}

static void display_rgb(miseru_look look, double rgb[3])
{
    double w[3], c[3], s = miseru_clamp01(look.sat);
    int i;
    cct_to_rgb(look.cct, w);
    hue_to_rgb(look.hue, c);
    for (i = 0; i < 3; i++)
        rgb[i] = w[i] * (1 - s) + c[i] * s;
}

void miseru_render_fixture(const miseru_fixture_type *type, miseru_look look, uint8_t *out)
{
    int has_dimmer = 0, has_xfade = 0, has_white = 0, i;
    double rgb[3], white = 0, intensity = miseru_clamp01(look.intensity), scale;
    for (i = 0; i < type->count; i++) {
        has_dimmer |= type->roles[i].kind == MISERU_DIMMER;
        has_xfade |= type->roles[i].kind == MISERU_XFADE;
        has_white |= type->roles[i].kind == MISERU_WHITE;
    }
    if (has_xfade) {
        hue_to_rgb(look.hue, rgb); /* the white side is the CCT channel's job */
    } else {
        display_rgb(look, rgb);
        if (has_white) {
            double max;
            white = fmin(rgb[0], fmin(rgb[1], rgb[2]));
            for (i = 0; i < 3; i++)
                rgb[i] -= white;
            max = fmax(white, fmax(rgb[0], fmax(rgb[1], rgb[2])));
            if (max > 0) {
                white /= max;
                for (i = 0; i < 3; i++)
                    rgb[i] /= max;
            }
        }
    }
    scale = has_dimmer ? 1.0 : intensity;
    for (i = 0; i < type->count; i++) {
        const miseru_role *r = &type->roles[i];
        switch (r->kind) {
        case MISERU_DIMMER: out[i] = to8(intensity); break;
        case MISERU_RED:    out[i] = to8(rgb[0] * scale); break;
        case MISERU_GREEN:  out[i] = to8(rgb[1] * scale); break;
        case MISERU_BLUE:   out[i] = to8(rgb[2] * scale); break;
        case MISERU_WHITE:  out[i] = to8(white * scale); break;
        case MISERU_CCT:    out[i] = to8((look.cct - r->a) / (double)(r->b - r->a)); break;
        case MISERU_TINT:   out[i] = (uint8_t)r->a; break;
        case MISERU_XFADE:  out[i] = to8(look.sat); break;
        case MISERU_FIXED:  out[i] = (uint8_t)r->a; break;
        }
    }
}

void miseru_show_init(miseru_show *s)
{
    memset(s, 0, sizeof(*s));
    s->master = 1.0;
}

int miseru_show_find(const miseru_show *s, const char *name)
{
    int i;
    for (i = 0; i < s->count; i++)
        if (strcmp(s->lights[i].name, name) == 0)
            return i;
    return -1;
}

int miseru_show_add(miseru_show *s, const char *name, const char *group, const char *type_id, int address)
{
    const miseru_fixture_type *type;
    miseru_light *l;
    int i, end;
    if (s->count >= MISERU_MAX_LIGHTS || !name[0] || miseru_show_find(s, name) >= 0)
        return -1;
    type = miseru_find_type(type_id);
    if (!type)
        return -2;
    end = address + type->count - 1;
    if (address < 1 || end > MISERU_SLOTS)
        return -3;
    for (i = 0; i < s->count; i++) {
        int o_end = s->lights[i].address + s->lights[i].type->count - 1;
        if (address <= o_end && end >= s->lights[i].address)
            return -4;
    }
    l = &s->lights[s->count++];
    memset(l, 0, sizeof(*l));
    strncpy(l->name, name, MISERU_NAME_LEN - 1);
    strncpy(l->group, group ? group : "", MISERU_NAME_LEN - 1);
    l->type = type;
    l->address = address;
    l->look = miseru_default_look;
    return 0;
}

void miseru_show_studio(miseru_show *s)
{
    static const char *spot[] = { "arri1", "arri2", "arri3" };
    static const char *panel[] = { "diva1", "diva2", "diva3", "diva4" };
    int i;
    miseru_show_init(s);
    for (i = 0; i < 3; i++)
        miseru_show_add(s, spot[i], "spot", "arri-l7c", 1 + 12 * i);
    for (i = 0; i < 4; i++)
        miseru_show_add(s, panel[i], "panel", "diva-lite", 37 + 8 * i);
}

void miseru_show_reset(miseru_show *s)
{
    int i;
    for (i = 0; i < s->count; i++)
        s->lights[i].look = miseru_default_look;
    s->master = 1.0;
    s->blackout = 0;
}

void miseru_show_all_on(miseru_show *s)
{
    int i;
    for (i = 0; i < s->count; i++)
        if (s->lights[i].look.intensity == 0)
            s->lights[i].look.intensity = 1.0;
    s->blackout = 0;
    if (s->master == 0)
        s->master = 1.0;
}

int miseru_show_matches(const miseru_show *s, int index, const char *target)
{
    const miseru_light *l = &s->lights[index];
    return strcmp(target, "all") == 0 || strcmp(target, l->name) == 0 || (l->group[0] && strcmp(target, l->group) == 0);
}

void miseru_show_render(const miseru_show *s, uint8_t *out512)
{
    int i;
    double factor = s->blackout ? 0.0 : miseru_clamp01(s->master);
    memset(out512, 0, MISERU_SLOTS);
    for (i = 0; i < s->count; i++) {
        const miseru_light *l = &s->lights[i];
        miseru_look look = l->look;
        look.intensity *= factor;
        miseru_render_fixture(l->type, look, out512 + l->address - 1);
    }
}

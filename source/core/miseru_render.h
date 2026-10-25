/*
 * miseru_render — "brightness, colour, warmth of white" -> DMX channel values.
 * A C port of the miseru engine (miseru-lighting: src/engine/color.ts, fixtures.ts, show.ts).
 * Keep both in step: tests/test_miseru.c checks the same cases as the TypeScript tests.
 * No Max or OS dependencies.
 * SPDX-License-Identifier: MIT
 */
#ifndef MISERU_RENDER_H
#define MISERU_RENDER_H

#include <stddef.h>
#include <stdint.h>

#define MISERU_SLOTS      512
#define MISERU_MAX_LIGHTS 64
#define MISERU_MAX_ROLES  16
#define MISERU_NAME_LEN   32

typedef struct {
    double intensity; /* 0..1 */
    double sat;       /* 0..1, 0 = white (warmth from cct), 1 = colour only */
    double hue;       /* 0..360 */
    double cct;       /* kelvin */
} miseru_look;

typedef enum {
    MISERU_DIMMER, MISERU_RED, MISERU_GREEN, MISERU_BLUE, MISERU_WHITE,
    MISERU_CCT,   /* a = min kelvin, b = max kelvin */
    MISERU_TINT,  /* a = neutral value */
    MISERU_XFADE,
    MISERU_FIXED  /* a = value */
} miseru_role_kind;

typedef struct {
    miseru_role_kind kind;
    int a, b;
} miseru_role;

typedef struct {
    const char *id;       /* name used in patch messages */
    int count;
    miseru_role roles[MISERU_MAX_ROLES];
} miseru_fixture_type;

typedef struct {
    char name[MISERU_NAME_LEN];
    char group[MISERU_NAME_LEN]; /* may be empty */
    const miseru_fixture_type *type;
    int address; /* 1-based */
    miseru_look look;
} miseru_light;

typedef struct {
    miseru_light lights[MISERU_MAX_LIGHTS];
    int count;
    double master;
    int blackout;
} miseru_show;

extern const miseru_look miseru_default_look; /* intensity 0, white, 5600 K */

const miseru_fixture_type *miseru_find_type(const char *id);
size_t miseru_type_count(void);
const miseru_fixture_type *miseru_type_at(size_t i);

/* Writes type->count values (0..255) to out. */
void miseru_render_fixture(const miseru_fixture_type *type, miseru_look look, uint8_t *out);

void miseru_show_init(miseru_show *s);
/* Returns 0 on success, -1 name taken / list full, -2 unknown type, -3 address out of range,
 * -4 channels overlap another light. */
int miseru_show_add(miseru_show *s, const char *name, const char *group, const char *type_id, int address);
/* Replaces the patch with the TMU photo studio (3 x ARRI, 4 x Diva-Lite). */
void miseru_show_studio(miseru_show *s);
void miseru_show_reset(miseru_show *s);
void miseru_show_all_on(miseru_show *s);
/* Index of the light with this name, or -1. */
int miseru_show_find(const miseru_show *s, const char *name);
/* Whether a light matches a target: "all", a group name, or a light name. */
int miseru_show_matches(const miseru_show *s, int index, const char *target);
/* Renders all lights into 512 slots. */
void miseru_show_render(const miseru_show *s, uint8_t *out512);

double miseru_clamp01(double v);

#endif

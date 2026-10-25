/*
 * Tests for miseru_render.c. The expected values come from the TypeScript engine
 * (miseru-lighting renderFixture) for the same inputs.
 * SPDX-License-Identifier: MIT
 */
#include "miseru_render.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while (0)

static void expect(const char *type, double i, double s, double h, double k, const uint8_t *want, int n)
{
    uint8_t got[16] = { 0 };
    miseru_look look = { i, s, h, k };
    int c;
    miseru_render_fixture(miseru_find_type(type), look, got);
    for (c = 0; c < n; c++) {
        if (got[c] != want[c]) {
            printf("FAIL %s i=%g s=%g h=%g k=%g: ch %d got %d want %d\n", type, i, s, h, k, c + 1, got[c], want[c]);
            failures++;
        }
    }
}

static void test_matches_typescript(void)
{
    static const uint8_t a[] = { 5, 4, 0, 255 };
    static const uint8_t b[] = { 255, 0, 0, 0 };
    static const uint8_t c[] = { 0, 29, 27, 179 };
    static const uint8_t d[] = { 0, 0, 128 };
    static const uint8_t e[] = { 204, 146, 93 };
    static const uint8_t f[] = { 128, 0, 0, 255 };
    static const uint8_t g[] = { 230, 255, 0, 168, 93 };
    static const uint8_t h[] = { 84 };
    static const uint8_t i[] = { 255, 99, 128, 255, 255, 0, 0, 0, 0, 0, 0, 0 };
    static const uint8_t j[] = { 153, 25, 128, 64, 0, 255, 0, 0, 0, 0, 0, 0 };
    static const uint8_t k[] = { 102, 224, 128, 204, 255, 191, 0, 0 };
    expect("rgbw", 1, 0, 0, 6500, a, 4);
    expect("rgbw", 1, 1, 0, 5600, b, 4);
    expect("rgbw", 0.7, 0.4, 200, 3200, c, 4);
    expect("rgb", 0.5, 1, 240, 5600, d, 3);
    expect("rgb", 0.8, 0.3, 30, 4000, e, 3);
    expect("dim-rgb", 0.5, 1, 240, 5600, f, 4);
    expect("dim-rgbw", 0.9, 0.6, 300, 2800, g, 5);
    expect("dimmer", 0.33, 0, 0, 5600, h, 1);
    expect("arri-l7c", 1, 1, 0, 5600, i, 12);
    expect("arri-l7c", 0.6, 0.25, 120, 3500, j, 12);
    expect("diva-lite", 0.4, 0.8, 45, 9000, k, 8);
}

static void test_patch_rules(void)
{
    miseru_show s;
    miseru_show_init(&s);
    CHECK(miseru_show_add(&s, "a", "g", "rgbw", 1) == 0);
    CHECK(miseru_show_add(&s, "a", "g", "rgbw", 9) == -1);   /* name taken */
    CHECK(miseru_show_add(&s, "b", "g", "nope", 9) == -2);   /* unknown type */
    CHECK(miseru_show_add(&s, "b", "g", "rgbw", 0) == -3);
    CHECK(miseru_show_add(&s, "b", "g", "rgbw", 510) == -3); /* runs past 512 */
    CHECK(miseru_show_add(&s, "b", "g", "rgbw", 4) == -4);   /* overlaps a */
    CHECK(miseru_show_add(&s, "b", "g", "rgbw", 5) == 0);
}

static void test_studio_and_commands(void)
{
    miseru_show s;
    uint8_t out[MISERU_SLOTS];
    int i;
    miseru_show_studio(&s);
    CHECK(s.count == 7);
    CHECK(s.lights[3].address == 37 && s.lights[6].address == 61);
    CHECK(miseru_show_matches(&s, 0, "spot") && !miseru_show_matches(&s, 3, "spot"));
    miseru_show_all_on(&s);
    miseru_show_render(&s, out);
    CHECK(out[0] == 255);     /* arri1 dimmer */
    CHECK(out[36] == 255);    /* diva1 dimmer */
    CHECK(out[60] == 255);    /* diva4 dimmer */
    for (i = 68; i < MISERU_SLOTS; i++)
        CHECK(out[i] == 0);
    s.blackout = 1;
    miseru_show_render(&s, out);
    CHECK(out[0] == 0 && out[36] == 0 && out[60] == 0);
    miseru_show_reset(&s);
    miseru_show_render(&s, out);
    CHECK(out[0] == 0 && s.blackout == 0 && s.master == 1.0);
}

int main(void)
{
    test_matches_typescript();
    test_patch_rules();
    test_studio_and_commands();
    if (failures) {
        printf("%d failures\n", failures);
        return 1;
    }
    printf("miseru_render: all passed\n");
    return 0;
}

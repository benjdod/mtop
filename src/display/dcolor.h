/* Copyright (c) 2022 Ben Dod 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. */

#ifndef MTOP_DISPLAY_DCOLOR_H
#define MTOP_DISPLAY_DCOLOR_H

#include "common.h"
#include "dstring.h"

#define DCOLOR_BLACK     0
#define DCOLOR_RED       1
#define DCOLOR_GREEN     2
#define DCOLOR_YELLOW    3
#define DCOLOR_BLUE      4
#define DCOLOR_MAGENTA   5
#define DCOLOR_CYAN      6
#define DCOLOR_WHITE     7

// stage (either foreground or background)
#define DCOLOR_FG        30     // foreground
#define DCOLOR_BG        40     // background

// nature (or mode)
#define DCOLOR_NORMAL    0      // normal
#define DCOLOR_UNSET     1      // inactive color. does not draw
#define DCOLOR_RESET     2      // sends a reset sequence
#define DCOLOR_BRIGHT    60     // bright color

typedef struct drgb_t_ {
    u8 r,g,b;
} drgb_t;

typedef struct dcolor_t_ {
    drgb_t rgb;

    // intended to represent the ansi set
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    u8
        hue,     
        stage,      // foreground or background
        nature;     // normal, bright, reset, or unset
} dcolor_t;

#define DCOLOR_SAMPLE_GREEN_FG  ((dcolor_t) {{0,255,0},     DCOLOR_GREEN,   DCOLOR_FG,  DCOLOR_NORMAL})
#define DCOLOR_SAMPLE_BLUE_FG   ((dcolor_t) {{0,0,255},     DCOLOR_BLUE,    DCOLOR_FG,  DCOLOR_NORMAL})
#define DCOLOR_SAMPLE_GREEN_BG  ((dcolor_t) {{0,50,0}, DCOLOR_BLUE, DCOLOR_BG, DCOLOR_NORMAL})
#define DCOLOR_SAMPLE_RESET     ((dcolor_t) {{0,0,0},       DCOLOR_WHITE,   DCOLOR_FG,  DCOLOR_RESET})
#define DCOLOR_SAMPLE_UNSET 	((dcolor_t) {{0,0,0}, 		DCOLOR_WHITE,   DCOLOR_FG,  DCOLOR_UNSET})
#define HIGHLIGHT_COLOR ((dcolor_t) {(drgb_t) {0,200,0}, DCOLOR_GREEN, DCOLOR_FG, DCOLOR_NORMAL})
#define BASE_COLOR ((dcolor_t) {(drgb_t) {100,100,100}, DCOLOR_WHITE, DCOLOR_FG, DCOLOR_NORMAL})

#define DCOLOR_WRITEBUFFER_LENGTH 24
#define DCOLOR_WRITEBUFFER_CALLED(NAME) char NAME[DCOLOR_WRITEBUFFER_LENGTH]; x_memset(NAME, 0, sizeof(NAME))
#define DCOLOR_WRITEBUFFER_CLEAR(NAME) x_memset(NAME, 0, sizeof(NAME))

size_t dcolor_write(dcolor_t color, char* buf, size_t n);
int dcolor_eq(dcolor_t a, dcolor_t b);

#endif

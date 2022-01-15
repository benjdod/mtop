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

#ifndef MTOP_DRAW_H
#define MTOP_DRAW_H

#include "proc.h"
#include "drawbuffer.h"

#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_YELLOW    3
#define COLOR_BLUE      4
#define COLOR_MAGENTA   5
#define COLOR_CYAN      6
#define COLOR_WHITE     7

#define COLOR_FG        30
#define COLOR_BG        40  
#define COLOR_NORMAL    0
#define COLOR_RESET     1
#define COLOR_BRIGHT    60

typedef struct rgb_t_ {
    unsigned char r,g,b;
} rgb_t;

typedef struct color_t_ {
    rgb_t rgb;

    // intended to represent the ansi set
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    unsigned char
        hue,     
        stage,      // foreground or background
        nature;     // normal or bright
} color_t;

/* draws a color to the buffer according to the draw settings (n must be >= 20 or does nothing.) */
size_t draw_color(color_t color, char* buf, size_t n);

size_t draw_queryrow(procs_info_t* info, char* buf, size_t n, size_t r_off, size_t c_off, int step);
void draw_fillbuffer(drawbuffer_t* dbuf, procs_info_t* procs, size_t r_size);

#endif

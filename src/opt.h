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

#ifndef MTOP_OPT_H
#define MTOP_OPT_H

#include "display/dcolor.h"
#include "common.h"

// option flags

#define OPT_YES 0x1
#define OPT_NO  0x0

#define OPT_DRAWCOLOR_NONE     0x0
#define OPT_DRAWCOLOR_ANSI     0x1
#define OPT_DRAWCOLOR_8BIT     0x2
#define OPT_DRAWCOLOR_24BIT    0x3

#define OPT_LOG_NONE            0x0
#define OPT_LOG_VERBOSE         0x1
#define OPT_LOG_DEBUG           0x2

typedef struct color_opt_t_ {
    u8 mode;
    dcolor_t stops[8];
    u32 num_stops;
    dcolor_t 
        head,
        base,
        highlight;
} color_opt_t;

typedef struct mtop_opt_t_ {
    u8 draw_static;
    u32 refresh_rate;
    color_opt_t color;  
    double falloff;
    u8 logging;
} mtop_opt_t;

extern mtop_opt_t opt;

#define get_opt(FIELD) (opt.FIELD)
#define set_opt(FIELD, VALUE) {opt.FIELD = VALUE;}

void opt_default();
void opt_print();

#endif

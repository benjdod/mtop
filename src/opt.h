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

#include "common.h"

#define OPT_YES 0x1
#define OPT_NO  0x0

typedef struct mtop_opt_t_ {
#define OPT_DRAWCOLOR_NONE     0x0
#define OPT_DRAWCOLOR_ANSI     0x1
#define OPT_DRAWCOLOR_8BIT     0x2
#define OPT_DRAWCOLOR_24BIT    0x3
    u8 colormode;
    u8 draw_static;
    u32 refresh_rate;
} mtop_opt_t;

extern mtop_opt_t opt;

#define SET_OPT(FIELD, VALUE) {opt.FIELD = VALUE;}

void opt_default();
void opt_print();

#endif

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

#include "opt.h"

mtop_opt_t opt;	// global options struct

void opt_default() {
    opt.colormode       = OPT_DRAWCOLOR_24BIT;
    opt.draw_static     = OPT_NO;
    opt.refresh_rate    = 100;
}

#define YESNO(B) (B) ? "yes" : "no"

#define COLORMODE_TOSTR(MODE) \
    (MODE == OPT_DRAWCOLOR_24BIT) \
        ? "24 bit" \
        : (MODE == OPT_DRAWCOLOR_8BIT) \
            ? "8 bit" \
            : (MODE = OPT_DRAWCOLOR_ANSI) \
                ? "ansi" \
                : "none" 

void opt_print() {
    printf("color mode:\t%s\n", COLORMODE_TOSTR(opt.colormode));
    printf("draw static:\t%s\n", YESNO(opt.draw_static));
    printf("refresh rate:\t%ums\n", opt.refresh_rate);
}

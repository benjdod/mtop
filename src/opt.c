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
    opt.draw_static     = OPT_NO;
    opt.refresh_rate    = 100;

    opt.falloff = 1.5;

    opt.color.mode       = OPT_DRAWCOLOR_24BIT;

    opt.color.head = (dcolor_t) {
        {255,255,255},
        DCOLOR_WHITE,
        DCOLOR_FG,
        DCOLOR_BRIGHT
    };

    opt.color.stops[0] = (dcolor_t) {
            {0,80,0},
            DCOLOR_GREEN,
            DCOLOR_FG,
            DCOLOR_NORMAL
        };
    opt.color.stops[1] = (dcolor_t) {
            {0,138,0},
            DCOLOR_GREEN,
            DCOLOR_FG,
            DCOLOR_NORMAL
        };
    opt.color.stops[2] = (dcolor_t) {
            {0,255,0},
            DCOLOR_GREEN,
            DCOLOR_FG,
            DCOLOR_BRIGHT
        };

    opt.color.num_stops = 3;

    opt.color.base = (dcolor_t) {(drgb_t) {100,100,100}, DCOLOR_WHITE, DCOLOR_FG, DCOLOR_NORMAL};
    opt.color.highlight = (dcolor_t) {(drgb_t) {0,200,0}, DCOLOR_GREEN, DCOLOR_FG, DCOLOR_NORMAL};
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

/* print important options */
void opt_print() {
    printf("color mode:\t%s\n", COLORMODE_TOSTR(get_opt(color.mode)));
    printf("draw static:\t%s\n", YESNO(opt.draw_static));
    printf("refresh rate:\t%ums\n", opt.refresh_rate);
}

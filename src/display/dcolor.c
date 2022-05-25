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

#include "dcolor.h"
#include "xutil.h"
#include "opt.h"

size_t dcolor_write(dcolor_t color, char* buf, size_t n) {

    // buf must be at least 24 to safely accomodate "\e[38;2;255;255;255m" + '\0';
	// colormode must be turned on
	// color nature must not be unset
    if (n < DCOLOR_WRITEBUFFER_LENGTH || get_opt(color.mode) == OPT_DRAWCOLOR_NONE || color.nature == DCOLOR_UNSET) return 0;  

	// RESET supercedes all 
    if (color.nature == DCOLOR_RESET) {
        x_strncpy(buf, "\e[0m", 4);
        return 4;
    } 
	
	// write according to colormode
	if ( get_opt(color.mode) ==OPT_DRAWCOLOR_24BIT) {
		// foreground:   <esc>[38;2;<r>;<g>;<b>m
		// background:   <esc>[48;2;<r>;<g>;<b>m
        return (size_t) snprintf(buf, 24, "\e[%d;2;%d;%d;%dm", 
				(color.stage == DCOLOR_BG) ? 48 : 38,
				color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        char colorcode = color.hue + color.nature + color.stage;
        return (size_t) snprintf(buf, 24, "\e[%dm", colorcode);
    }
}

int dcolor_eq(dcolor_t a, dcolor_t b) {
	if (get_opt(color.mode) == OPT_DRAWCOLOR_NONE) return 1;
	else if (get_opt(color.mode) == OPT_DRAWCOLOR_ANSI) {
		return (a.nature == b.nature &&
				a.hue == b.hue &&
				a.stage == b.stage);
	} else {
		return (a.nature== b.nature&&
				a.rgb.r == b.rgb.r &&
				a.rgb.g == b.rgb.g &&
				a.rgb.b == b.rgb.b &&
				a.hue   == b.hue   &&
				a.stage == b.stage) ? 1 : 0;
	}
}

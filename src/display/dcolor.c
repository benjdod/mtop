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

#define DCOLOR_USECOLOR 1
#define DCOLOR_USETRUECOLOR 0

size_t dcolor_write(dcolor_t color, char* buf, size_t n) {

    // buf must be at least 20 to accomodate "\e[38;2;255;255;255m" + '\0';
    if (n < 20 || !(DCOLOR_USECOLOR | DCOLOR_USETRUECOLOR)) return 0;  

    if (color.nature == DCOLOR_RESET) {
        x_strncpy(buf, "\e[0m", 4);
        return 4;
    } else if (DCOLOR_USETRUECOLOR) {
        return (size_t) snprintf(buf, 19, "\e[38;2;%d;%d;%dm", color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        char colorcode = color.hue + color.nature + color.stage;
        return (size_t) snprintf(buf, 19, "\e[%dm", colorcode);
    }
}
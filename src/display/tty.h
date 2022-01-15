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

#ifndef _TTY_H
#define _TTY_H

#include "common.h"

#define TTY_TRAPSIGNAL  0x0001

#define TTY_EBADDEV     0x0001       // output device is not a tty
#define TTY_ETCATTR     0x0002       // bad return from get/set tc attr funcs

/* sets the tty into raw mode
*  reads characters one at a time, and doesn't 
*  generate signals */
void tty_setraw();

/* sets the tty back to the configuration
*  it was in before tty_screen_open) was called */
void tty_reset();

/* flush output from current tty */
void tty_oflush();
/* flush input from current tty */
void tty_iflush();

/* returns an integer indicating whether or not 
*  the tty is in raw mode.
*  0: not raw
*  non-zero: raw */
int tty_israw();

char tty_readc();

/* writes a null-terminated string to
*  the tty display */
int tty_writes(const char *);

/* writes n bytes starting at the pointer
*  to the tty display */
int tty_writesn(const char *, int);

/* writes an integer to the tty display */
int tty_writed(int);

/* writes a single character to the tty display */
int tty_writec(char);

/* fill display with n repetitions of c */
int tty_fill(char, int);
int tty_clear();

#endif

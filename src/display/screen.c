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

#include "common.h"

#include "screen.h"
#include "error.h"
#include "tty.h"

static int open = 0;

screensize_t get_screensize() {
	screensize_t sz;

	sz.rows = 0;
	sz.cols = 0;

	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)) {
		fatal(1,"could not get window size");
	}

	sz.rows = ws.ws_row;
	sz.cols = ws.ws_col;

	return sz;
}

void screen_open() {
	tty_setraw();
	tty_writes("\e[?1049h");
	tty_clear();
	open = 1;
}

void screen_exit() {
	tty_writes("\e[?1049l");
	tty_reset();
	open = 0;
}

int screen_isopen() {
	return open;
}

void screen_clear() {
	tty_writes("\e[2J");
	tty_writes("\e[0;0H");
}

void screen_setcursor(rowcol_t location) {
	char buf[32];

	snprintf(buf, 32, "\e[%u;%uH", location.rows + 1, location.cols + 1);
	tty_writes(buf);
}

void screen_hidecursor() {
	tty_writesn("\e[?25l", 6);
}

void screen_showcursor() {
	tty_writesn("\e[?25h", 6);
}

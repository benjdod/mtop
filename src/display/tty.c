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

#include <termios.h>

#include "common.h"
#include "tty.h"
#include "error.h"
#include "xutil.h"

// TTY flags
#define TTY_ISRAW 0x1

static struct termios orig_termios;
static int ttyfd = STDIN_FILENO;
static int israw = 0;
static u16 flags;

/*** STATIC FUNCTIONS ***/

static int tty_init() {

	// verifies that the program is running on a tty
	if (!isatty(ttyfd)) {
		fatal(1,"not on a tty");
		return TTY_EBADDEV;
	}

	// saves the current tty settings in `orig_termios`
	if (tcgetattr(ttyfd, &orig_termios) < 0) {
		fatal(1,"can't get tty settings");
		return TTY_ETCATTR;
	}

	// zero flags
	flags = 0;

	return 0;
}

void tty_setraw() {

	// init and sanity checks (see above)
	tty_init();


	// make a new termios struct to become raw
	//		https://man7.org/linux/man-pages/man3/termios.3.html
	struct termios raw = orig_termios;

	/* BRKINT 	- a break doesn't flush input and output queues
	 * ICRNL 	- doesn't \r to \n on input
	 * INPCK 	- disables input parity checking
	 * ISTRIP	- doesn't strip off 8th bit
	 * IXON		- disables xon/xoff (software) control flow
	 *
	 * OPOST	- disables implementation defined output processing
	 *
	 * CS8		- sets character mask size to 8 bits
	 *
	 * ECHO		- does not echo input characters
	 * ICANON	- disables canonical mode
	 * IEXTEN	- disables implementation defined input processing
	 * ISIG		- disables signal generation from characters INTR, QUIT, SUSP, and DSUSP
	 * */

	raw.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO|ICANON|IEXTEN /*|ISIG */ );

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(ttyfd, TCSAFLUSH, &raw) < 0) 
		fatal(1,"can't set raw mode");

	israw = 1;

	flags |= TTY_ISRAW;

	return;
}

int tty_israw() {
	return flags & TTY_ISRAW;
}

void tty_oflush() {
	tcflush(STDOUT_FILENO, TCOFLUSH);
}

void tty_iflush() {
	tcflush(STDIN_FILENO, TCIFLUSH);
}

void tty_reset() {

	// if the TTY isn't raw, we don't have to do anything here
	if (!(flags & TTY_ISRAW)) return;
	
	if (tcsetattr(ttyfd, TCSAFLUSH, &orig_termios) < 0) {
		printf("cannot reset tty (danger!)\n");
	}

	// turn off the ISRAW flag
	flags &= ~(TTY_ISRAW);

	// and make sure the cursor is visible and resets all graphics and colors
	tty_writes("\e[?25h");
	tty_writes("\e[0m");

	return;
}

char tty_readc() {
	char c = '\0';
	int readn = read(STDIN_FILENO, &c, 1);

	if (readn == -1) {
		fatal(1, "readkey");
		return '\0';
	} else {
		return c;
	}
}

int tty_writec(char c) {
	char cp = c;
	return write(STDOUT_FILENO,&cp,1);
}

int tty_writes(const char *str) {

	size_t i = strlen(str);

	return write(STDOUT_FILENO,str,i);
	/* this is evil, fot debugging only

	size_t i = strlen(str);
	size_t x = 0;
	for (x = 0; x < i; x++) {
		tty_writed(str[x]);
	}
	*/
}

int tty_writed(int d) {
	char s[128];

	snprintf(s,128,"%d",d);

	return tty_writes(s);
}

int tty_fill(char c, int n) {
	char buf[n];
	x_memset(buf, c, n);
	return write(STDOUT_FILENO, buf, n);
}

int tty_writesn(const char *str, int nbytes) {
	return write(STDOUT_FILENO,str,nbytes);
}

int tty_clear() {
	return write(STDOUT_FILENO,"\e[2J\e[H",7);
}

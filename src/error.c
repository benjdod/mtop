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
#include "opt.h"
#include "error.h"
#include "tty.h"

void fatal(int e_no, const char *msg, ...) {

	va_list args;
	va_start(args,msg);

	if (!tty_israw()) {
		printf("Fatal error (code %d): ",e_no);
		vprintf(msg,args);
		putchar('\n');
		va_end(args);
		return;
	}

	tty_clear();

	char cp[4096];

	size_t pos = 0;

	pos += snprintf(cp,4096,"Fatal error (code %d): ",e_no);

	pos += vsnprintf(cp+pos,4096-pos-3,msg,args);
	va_end(args);

	snprintf(cp+pos,3,"\r\n");

	tty_writes(cp);

	tty_writes("\r\nPress any key to exit...\r\n");

	char c;

	while (1) {
		ssize_t r = read(STDIN_FILENO,&c,1);

		if ((r == -1) || (r)) {
			break;
		}
	}

	tty_reset();

	exit(e_no);
}

void error(int e_no, const char *msg, ...) {
	va_list args;
	va_start(args,msg);

	if (!tty_israw()) {
		printf("Error: ");
		vprintf(msg,args);
		putchar('\n');
		va_end(args);
		exit(e_no);
	}

	tty_clear();

	char cp[4096];

	size_t pos = 0;

	pos += snprintf(cp,4096,"Error (code %d): ",e_no);
	pos += vsnprintf(cp+pos,4096-pos-3,msg,args);
	snprintf(cp+pos,3,"\r\n");
	va_end(args);

	tty_writes(cp);

	tty_writes("\r\nPress ctrl + c to exit...\n");

	char c;

	while (1) {
		ssize_t r = read(STDIN_FILENO,&c,1);
		
		if ((r == -1) || (r)) {
			break;
		}
	}

	tty_reset();

	exit(e_no);
}

void warn(char *msg) {
	fatal(1000,msg);
}

void notify(char *msg) {
	fatal(2000,msg);
}

void verbose(const char* format, ...) {
	if (get_opt(logging) < OPT_LOG_VERBOSE) return;
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void debug(const char* format, ...) {
	if (get_opt(logging) < OPT_LOG_DEBUG) return;
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
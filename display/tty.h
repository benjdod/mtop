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

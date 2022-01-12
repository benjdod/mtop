#ifndef MTOP_SCREEN_H
#define MTOP_SCREEN_H

#include "common.h"

#include "tty.h"

typedef struct rowcol_t_ {
	uint16_t rows;
	uint16_t cols;
} rowcol_t;

typedef rowcol_t screensize_t;

screensize_t get_screensize();
void screen_open();
void screen_exit();
int screen_isopen();
void screen_clear();

void screen_setcursor(rowcol_t location);
void screen_hidecursor();
void screen_showcursor();

#endif

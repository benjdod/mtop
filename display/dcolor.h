#ifndef MTOP_DISPLAY_DCOLOR_H
#define MTOP_DISPLAY_DCOLOR_H

#include "common.h"
#include "dstring.h"

#define DCOLOR_BLACK     0
#define DCOLOR_RED       1
#define DCOLOR_GREEN     2
#define DCOLOR_YELLOW    3
#define DCOLOR_BLUE      4
#define DCOLOR_MAGENTA   5
#define DCOLOR_CYAN      6
#define DCOLOR_WHITE     7

#define DCOLOR_FG        30
#define DCOLOR_BG        40  
#define DCOLOR_NORMAL    0
#define DCOLOR_RESET     1
#define DCOLOR_BRIGHT    60

typedef struct drgb_t_ {
    u8 r,g,b;
} drgb_t;

typedef struct dcolor_t_ {
    drgb_t rgb;

    // intended to represent the ansi set
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    u8
        hue,     
        stage,      // foreground or background
        nature;     // normal or bright
} dcolor_t;

size_t dcolor_write(dcolor_t color, char* buf, size_t n);

#endif
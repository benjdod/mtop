#ifndef _CMTOP_DRAW_H
#define _CMTOP_DRAW_H

#include "proc.h"

#define DRAW_COLOR      0x0001
#define DRAW_RGBCOLOR   0x0002

#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_YELLOW    3
#define COLOR_BLUE      4
#define COLOR_MAGENTA   5
#define COLOR_CYAN      6
#define COLOR_WHITE     7

#define COLOR_FG        30
#define COLOR_BG        40  
#define COLOR_NORMAL    0
#define COLOR_RESET     1
#define COLOR_BRIGHT    60

typedef struct rgb_t_ {
    unsigned char r,g,b;
} rgb_t;

typedef struct color_t_ {
    rgb_t rgb;

    // intended to represent the ansi set
    // https://en.wikipedia.org/wiki/ANSI_escape_code
    unsigned char
        hue,     
        stage,      // foreground or background
        nature;     // normal or bright
} color_t;

void draw_setopts(uint16_t flags);

/* draws a color to the buffer according to the draw settings (n must be >= 19 or does nothing.) */
size_t draw_color(color_t color, char* buf, size_t n);

size_t draw_queryrow(procs_info_t* info, char* buf, size_t n, size_t r_off, size_t c_off, int step);
size_t draw_queryscr(procs_info_t* info, char* scrbuf, size_t r_size, size_t c_size, size_t r_off, size_t c_off, int c_step);

#endif
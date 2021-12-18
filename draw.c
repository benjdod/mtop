#include "common.h"
#include "xutil.h"
#include "draw.h"
#include "proc.h"
#include "procdraw.h"

static uint16_t flags = 0;

void draw_setopts(uint16_t f) {
    flags = f;
}

static unsigned int isset(unsigned int flag) {
    return flags & flag;
}

size_t draw_color(color_t color, char* buf, size_t n) {
    //"\e[38;2;255;255;255m";     // 19 chars!

    if (n < 19 || !(isset(DRAW_COLOR | DRAW_RGBCOLOR))) return 0;

    if (color.nature == COLOR_RESET) {
        x_strncpy(buf, "\e[0m", 4);
        return 4;
    } else if (isset(DRAW_RGBCOLOR)) {
        return snprintf(buf, 19, "\e[38;2;%d;%d;%dm", color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        char colorcode = color.hue + color.nature + color.stage;
        return snprintf(buf, 19, "\e[%dm", colorcode);
    }
}

size_t draw_queryrow(procs_info_t* info, char* buf, size_t n, size_t r_off, size_t c_off, int step) {

    size_t i = 0;

    if (step < 1) step = 1;

    procbst_cursor_t cur = procbst_cursor_init(&info->procs);
    procbst_cursor_next(&cur);

    for (size_t j = 0; j < c_off; j++) { procbst_cursor_next(&cur); }

    while (cur.current != NULL && i < n) {
        buf[i*step] = pd_charat(&(cur.current->value), r_off);
        procbst_cursor_next(&cur);
        i++;
    }

    return i*step;
}

size_t draw_queryscr(procs_info_t* info, char* scrbuf, size_t r_size, size_t c_size, size_t r_off, size_t c_off, int c_step) {

    if (c_step < 1) c_step = 1;

    procbst_cursor_t cur = procbst_cursor_init(&info->procs);
    procbst_cursor_next(&cur);

    for (size_t j = 0; j < c_off; j++) { procbst_cursor_next(&cur); }

    for (size_t c = 0; c < c_size; c++) {
        for (size_t r = 0; r < r_size; r++) {

            if (c % c_step == 0) {
                scrbuf[r * c_size + c] = (cur.current != NULL)
                    ? /* 'a' + (c % 26) */ pd_charat(&(cur.current->value), r + r_off)
                    : ' ';
            } else {
                scrbuf[r*c_size + c] = ' ';
            }
        }
        if (cur.current != NULL) procbst_cursor_next(&cur);
    }

    return r_size * c_size;
}
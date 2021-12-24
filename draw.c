#include "common.h"
#include "xutil.h"
#include "draw.h"
#include "proc.h"
#include "procdraw.h"
#include "drawbuffer.h"

static uint16_t flags = 0;

void draw_setopts(uint16_t f) {
    flags = f;
}

static unsigned int isset(unsigned int flag) {
    return flags & flag;
}

static char randchar() {
    char r_chars[] = {
        '\'',
        '`',
        '.',
        ','
    };

    u8 rselect = (rand() % (sizeof(r_chars) * 3));

    return (rselect < sizeof(r_chars)) ? r_chars[rselect] : ' ';
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

    while (i < n) {
        buf[i*step] = '-';
        i++;
    }

    return i*step;
}

size_t draw_queryscr(procs_info_t* info, char* scrbuf, size_t r_size, size_t c_size, size_t r_off, size_t c_off, int c_step) {

    if (c_step < 1) c_step = 1;

    procbst_cursor_t cur = procbst_cursor_init(&info->procs);
    procbst_cursor_next(&cur);

    unsigned char info_winsz = 2;

    for (size_t j = 0; j < c_off; j++) { procbst_cursor_next(&cur); }

    for (size_t c = 0; c < c_size; c++) {

        u8 draw_a_proc = (c % c_step == 0) ? 1 : 0;

        for (size_t r = 0; r < r_size - info_winsz; r++) {

            if (draw_a_proc) {
                scrbuf[r * c_size + c] = (cur.current != NULL)
                    ? /* 'a' + (c % 26) */ pd_charat(&(cur.current->value), r + r_off)
                    : randchar();
            } else {
                scrbuf[r*c_size + c] = ' ';
            }
        }

        if (draw_a_proc && cur.current != NULL) {
            procbst_cursor_next(&cur);
        }
    }

    size_t w_len = 0;
    char* w_ptr;

    w_ptr = scrbuf + ((r_size - info_winsz) * c_size);
    w_len = pd_drawinfo(&info->selected.current->value, w_ptr, c_size, 0);

    while (w_len < c_size) {
        w_ptr[w_len++] = ' ';
    }

    w_ptr = scrbuf + (r_size - info_winsz + 1) * c_size;
    w_len = pd_drawinfo(&info->selected.current->value, w_ptr, c_size, 1);

    while (w_len < c_size) {
        w_ptr[w_len++] = ' ';
    }

    return r_size * c_size;
}

void draw_fillbuffer(drawbuffer_t* dbuf, procs_info_t* info, size_t r_size, size_t c_size) {

    char tbuf[c_size+1];

    size_t 
        c_step = 2,
        r_step = 0,
        c_off = 0,
        r_off = 0;

    size_t sel_idx = 0;

    size_t info_winsz = 3;

#define SET_PRIMARYCOLOR() dbuf_adds(dbuf, "\e[38;2;0;200;0m");
#define SET_SECONDARYCOLOR() dbuf_adds(dbuf, "\e[0m\e[38;5;242m")

    // append matrix rows
    for (size_t r = 0; r < r_size - info_winsz; r++) {
        procbst_cursor_t cur = procbst_cursor_init(&info->procs);
        procbst_cursor_next(&cur);

        sel_idx = 0;

        for (size_t c = 0; c < c_size; c++) {
            if (c % c_step == 0 && cur.current != NULL) {
                if (!sel_idx && procbst_cursor_eq(cur, info->selected)) {
                    sel_idx = c;
                }
                tbuf[c] = pd_charat(procbst_cursor_at(&cur), r + r_off);
                procbst_cursor_next(&cur);
            } else {
                tbuf[c] = ' ';
            }
        }

        dcolor_t dc;
        dc.stage = DCOLOR_BG;
        dc.hue = DCOLOR_GREEN;
        dc.nature = DCOLOR_NORMAL;
        dc.rgb = (drgb_t) {0x0, 0x0, 0x0};

        dcolor_t reset;
        dc.stage = DCOLOR_BG;
        dc.nature = DCOLOR_RESET;
        dc.hue = DCOLOR_WHITE;
        dc.rgb = (drgb_t) {0x0, 0x0, 0x0};

        dbuf_addsn(dbuf, tbuf, sel_idx);
        //dbuf_addcolor(dbuf, dc);
        SET_PRIMARYCOLOR();
        dbuf_addc(dbuf, tbuf[sel_idx]);
        //dbuf_addcolor(dbuf, reset);
        //dbuf_adds(dbuf, "\e[32m");
        SET_SECONDARYCOLOR();
        dbuf_addsn(dbuf, tbuf + sel_idx + 1, c_size - sel_idx - 1);
    }

    SET_PRIMARYCOLOR();

    for (size_t i = 0; i < info_winsz; i++) {
        memset(tbuf, ' ', c_size);
        size_t w = pd_drawinfo(&info->selected.current->value, tbuf, c_size, i);
        while (w < c_size) {
            tbuf[w++] = ' ';
        }
        dbuf_addsn(dbuf, tbuf, c_size);
    }

    SET_SECONDARYCOLOR();
}
#include "common.h"
#include "xutil.h"
#include "opt.h"
#include "draw.h"
#include "proc.h"
#include "proclist.h"
#include "procdraw.h"
#include "drawbuffer.h"
#include "opt.h"

static uint16_t flags = 0;

void draw_setopts(uint16_t f) {
    flags = f;
}

/*
static unsigned int isset(unsigned int flag) {
    return flags & flag;
} */

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

    if (n < 19 || opt.colormode == OPT_DRAWCOLOR_NONE) return 0;

    if (color.nature == COLOR_RESET) {
        x_strncpy(buf, "\e[0m", 4);
        return 4;
    } else if (opt.colormode == OPT_DRAWCOLOR_24BIT) {
        return snprintf(buf, 19, "\e[38;2;%d;%d;%dm", color.rgb.r, color.rgb.g, color.rgb.b);
    } else {
        char colorcode = color.hue + color.nature + color.stage;
        return snprintf(buf, 19, "\e[%dm", colorcode);
    }
}

size_t draw_queryrow(procs_info_t* info, char* buf, size_t n, size_t r_off, size_t c_off, int step) {

    size_t i = 0;

    if (step < 1) step = 1;

    proclist_cur_t cur = pl_cur_init(&info->procs);
    pl_cur_next(&cur);

    for (size_t j = 0; j < c_off; j++) { pl_cur_next(&cur); }

    while (cur.current != NULL && i < n) {
        buf[i*step] = pd_charat(&(cur.current->value), r_off);
        pl_cur_next(&cur);
        i++;
    }

    while (i < n) {
        buf[i*step] = '-';
        i++;
    }

    return i*step;
}

void draw_fillbuffer(drawbuffer_t* dbuf, procs_info_t* info, size_t r_size, size_t c_size) {

    char tbuf[c_size+1];

    size_t 
        c_step = 2,
        //r_step = 0,
        //c_off = 0,
        r_off = 0;

    size_t sel_idx = 0;

    size_t info_winsz = 3;

#define SET_PRIMARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[38;2;0;200;0m");}
#define SET_SECONDARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[0m\e[38;5;242m");}

    // append matrix rows
    for (size_t r = 0; r < r_size - info_winsz - 1; r++) {  // for row in "window"
        proclist_cur_t cur = pl_cur_init(&info->procs);
        procinfo_t* pi = pl_cur_next(&cur);
        sel_idx = 0;

        u8 on_step = 0;

        for (size_t c = 0; c < c_size; c++) {   // for column in row
            on_step = c % c_step == 0 ? 1 : 0;
            if (pi != NULL) {
                if (on_step) {
                    if (!sel_idx && pl_cur_eq(&cur, &info->selected)) {
                        sel_idx = c;
                    }
                    tbuf[c] = pd_charat(pi, r + r_off);
                    pi = pl_cur_next(&cur);
                } else {
                    tbuf[c] = ' ';
                }
            } else {
                tbuf[c] = (on_step && opt.draw_static) ? randchar() : ' ';
            }
            
        }

        dbuf_addsn(dbuf, tbuf, sel_idx);
        //dbuf_addcolor(dbuf, dc);
        SET_PRIMARYCOLOR();
        dbuf_addc(dbuf, tbuf[sel_idx]);
        //dbuf_addcolor(dbuf, reset);
        //dbuf_adds(dbuf, "\e[32m");
        SET_SECONDARYCOLOR();
        dbuf_addsn(dbuf, tbuf + sel_idx + 1, c_size - sel_idx - 1);
    }
    for (size_t i = 0; i < c_size; i++) dbuf_addc(dbuf, '-');

    SET_PRIMARYCOLOR();

    for (size_t i = 0; i < info_winsz; i++) {
        x_memset(tbuf, ' ', c_size);
        size_t w = pd_drawinfo(pl_cur_at(&info->selected), tbuf, c_size, i);
        while (w < c_size) {
            tbuf[w++] = ' ';
        }
        dbuf_addsn(dbuf, tbuf, c_size);
    }

    SET_SECONDARYCOLOR();
}
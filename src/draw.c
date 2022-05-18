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
#include "xutil.h"
#include "opt.h"
#include "draw.h"
#include "proc.h"
#include "proclist.h"
#include "procdraw.h"
#include "drawbuffer.h"
#include "opt.h"

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

static size_t draw_system_info(drawbuffer_t* dbuf, procs_info_t* info, size_t r_size, size_t rows) {

#define CHECK_RETURN() {if (rows > 0 && rows_printed >= rows ) return rows;}
	size_t rows_printed = 0;
	size_t w = 0;

	char buf[r_size + 1];

	meminfo_t* memp = &info->sys.mem;

	// XXX: this smells pretty bad

	w = snprintf(buf, r_size, "Tasks: %lu/%lu, %lu running", info->selected_index + 1, info->sys.num_procs, info->sys.running);
	dbuf_addsn(dbuf, buf, w);
	dbuf_addcn(dbuf, ' ', r_size - w);

	rows_printed++;
	CHECK_RETURN();

	w = snprintf(buf, r_size, "Mem: %lu/%lu, swap: %lu/%lu", memp->free, memp->total, memp->swap_free, memp->swap_total);
	dbuf_addsn(dbuf, buf, w);
	dbuf_addcn(dbuf, ' ', r_size - w);

	rows_printed++;
	CHECK_RETURN();
	
	if (rows != 0 && rows > rows_printed) {
		dbuf_addcn(dbuf, ' ', r_size * (rows - rows_printed));
		rows_printed += rows - rows_printed;
	}

	return rows_printed;
}

/**
 * fills a drawbuffer with screen content. 
 * This is the main screen drawing routine.
 * */
void draw_fillbuffer(drawbuffer_t* dbuf, procs_info_t* info, size_t r_size) {

    char buf[info->display_size + 1];

	if (info->selected_index < info->col_offset) {
		info->col_offset = info->selected_index;
	} else if (info->selected_index >= info->col_offset + info->real_size) {
		info->col_offset = info->selected_index - info->real_size + 1;
	} 

    // FIXME: builtin color drawing doesnt work :(
//#define SET_PRIMARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[38;2;0;200;0m");}	
//#define SET_SECONDARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[0m\e[38;5;242m");}
#define SET_PRIMARYCOLOR() if (opt.colormode) {dbuf_addcolor(dbuf, HIGHLIGHT_COLOR);}
#define SET_SECONDARYCOLOR() if (opt.colormode) {dbuf_addcolor(dbuf, DCOLOR_SAMPLE_RESET); dbuf_addcolor(dbuf, BASE_COLOR);}
//#define DRAW_HORIZONTAL_SEP() dbuf_addcn(dbuf, '-', info->display_size)
#define DRAW_HORIZONTAL_SEP() for (size_t i = 0; i < info->display_size; i++) dbuf_adds(dbuf, "\u2501");

	size_t matrix_view_winsz = r_size;

	// eat space from matrix window for
	// sys info + horizontal sep and 
	// selected info + horiz sep 

    if (info->open_windows & PROCS_WINDOW_SYSINFO) {
		SET_SECONDARYCOLOR();
        size_t sys_info_winsz = 2;
        matrix_view_winsz -= (sys_info_winsz + 1);
        // draw system info window
        draw_system_info(dbuf, info, info->display_size, sys_info_winsz);
        DRAW_HORIZONTAL_SEP();
    }

    size_t selected_info_winsz = (info->open_windows & PROCS_WINDOW_PROCINFO) ? 3 : 0;

    if (selected_info_winsz > 0) {
        matrix_view_winsz -= (selected_info_winsz + 1);
    }

	proclist_cur_t cursor = pl_cur_init(&info->procs);
	pl_cur_next(&cursor);
	
	// FIXME: something gets messed up here where the col_offset gets flipped to be way more
	// which causes an infinite loop
	for (size_t i = 0; i < info->col_offset; i++) {
		pl_cur_next(&cursor);
	} 

	int skip_drawing = 0;

    // append matrix rows
	ssize_t sel_visual_idx = -1;
    for (size_t r = 0; r < matrix_view_winsz; r++) {  // for row in "window"
        proclist_cur_t cur = pl_cur_clone(&cursor);
		sel_visual_idx = -1;

        u8 on_step = 0;

        for (size_t c = 0; c < info->display_size; c++) {   // for column in row
			cchar_t cchar;
			cchar.c = ' ';
			cchar.color = DCOLOR_SAMPLE_UNSET;
			if (skip_drawing) {
				buf[c] = ' ';
				continue;
			}
            on_step = c % info->step == 0 ? 1 : 0;
            if (pl_cur_at(&cur) != NULL) {
                if (on_step) {
                    if (sel_visual_idx == -1 && pl_cur_eq(&cur, &info->selected)) {
                        sel_visual_idx = c;
                    }
					cchar = pd_ccharat(PL_CURVAL(&cur), r  + info->row_offset);
                    // buf[c] = pd_charat(PL_CURVAL(&cur), r  + info->row_offset );
					buf[c] = cchar.c;
					pl_cur_next(&cur);
                } else {
                    buf[c] = ' ';
                }
            } else {
                buf[c] = (on_step && opt.draw_static) ? randchar() : ' ';
            }

			int draw_as_selected = (sel_visual_idx == c && 
					info->open_windows & PROCS_WINDOW_PROCINFO);

			if (draw_as_selected) {
				dbuf_addcolor(dbuf, DCOLOR_SAMPLE_GREEN_BG);
			}

			if (cchar.color.nature != DCOLOR_UNSET) dbuf_addcolor(dbuf, cchar.color);
			dbuf_addc(dbuf, buf[c]);

			if (draw_as_selected) {
				dbuf_addcolor(dbuf, DCOLOR_SAMPLE_RESET);
			}
        }

		/*
		if (sel_visual_idx != -1) {
			dbuf_addsn(dbuf, buf, sel_visual_idx);
			SET_PRIMARYCOLOR();
			dbuf_addc(dbuf, buf[sel_visual_idx]);
			SET_SECONDARYCOLOR();
			dbuf_addsn(dbuf, buf + sel_visual_idx + 1, info->display_size - sel_visual_idx - 1);
		} else {
			dbuf_addsn(dbuf, buf, info->display_size);
		} */
    }

	// draw info window (this contains details for
	// the selected process
    if (info->open_windows & PROCS_WINDOW_PROCINFO) {
        SET_SECONDARYCOLOR();
        DRAW_HORIZONTAL_SEP();
        for (size_t i = 0; i < selected_info_winsz; i++) {
            x_memset(buf, ' ', info->display_size);
            pd_drawinfo(pl_cur_at(&info->selected), buf, info->display_size, i);
            /*while (w < info->display_size) {
                buf[w++] = ' ';
            }*/
            dbuf_addsn(dbuf, buf, info->display_size);
        }
    }
}

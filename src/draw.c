#include "common.h"
#include "xutil.h"
#include "opt.h"
#include "draw.h"
#include "proc.h"
#include "proclist.h"
#include "procdraw.h"
#include "drawbuffer.h"
#include "opt.h"

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

/**
 * fills a drawbuffer with screen content. 
 * This is the main screen drawing routine.
 * */
void draw_fillbuffer(drawbuffer_t* dbuf, procs_info_t* info, size_t r_size) {

    char buf[info->display_size + 1];

	if (info->selected_index < info->draw_offset) {
		info->draw_offset = info->selected_index;
	} else if (info->selected_index >= info->draw_offset + info->real_size) {
		info->draw_offset = info->selected_index - info->real_size + 1;
	} 

	size_t sys_info_winsz = 1;
    size_t selected_info_winsz = 3;

	size_t matrix_view_winsz = r_size;

	// eat space from matrix window for
	// sys info + horizontal sep and 
	// selected info + horiz sep 

	matrix_view_winsz -= (sys_info_winsz + selected_info_winsz + 2);

// FIXME: builtin color drawing doesnt work :(
#define SET_PRIMARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[38;2;0;200;0m");}	
#define SET_SECONDARYCOLOR() if (opt.colormode) {dbuf_adds(dbuf, "\e[0m\e[38;5;242m");}
#define DRAW_HORIZONTAL_SEP() {for (size_t i = 0; i < info->display_size; i++) dbuf_addc(dbuf, '-');}

	for (size_t i = 0; i < sys_info_winsz; i++) {
		size_t w = snprintf(buf, info->display_size, "Tasks: %lu/%lu, %lu running", info->selected_index + 1, info->sys.num_procs, info->sys.running);
		dbuf_addsn(dbuf, buf, w);
		for (int j = w; j < info->display_size; j++) {
			dbuf_addc(dbuf, ' ');
		}
	}
	DRAW_HORIZONTAL_SEP();

	proclist_cur_t cursor = pl_cur_init(&info->procs);
	pl_cur_next(&cursor);
	
	for (size_t i = 0; i < info->draw_offset; i++) {
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
                    buf[c] = pd_charat(PL_CURVAL(&cur), r /* + r_off */);
					pl_cur_next(&cur);
                } else {
                    buf[c] = ' ';
                }
            } else {
                buf[c] = (on_step && opt.draw_static) ? randchar() : ' ';
            }
            
        }

		if (sel_visual_idx != -1) {
			dbuf_addsn(dbuf, buf, sel_visual_idx);
			SET_PRIMARYCOLOR();
			dbuf_addc(dbuf, buf[sel_visual_idx]);
			SET_SECONDARYCOLOR();
			dbuf_addsn(dbuf, buf + sel_visual_idx + 1, info->display_size - sel_visual_idx - 1);
		} else {
			dbuf_addsn(dbuf, buf, info->display_size);
		}
    }

	// draw info window (this contains details for
	// the selected process
	DRAW_HORIZONTAL_SEP();
    SET_PRIMARYCOLOR();
    for (size_t i = 0; i < selected_info_winsz; i++) {
        x_memset(buf, ' ', info->display_size);
        size_t w = pd_drawinfo(pl_cur_at(&info->selected), buf, info->display_size, i);
        while (w < info->display_size) {
            buf[w++] = ' ';
        }
        dbuf_addsn(dbuf, buf, info->display_size);
    }
    SET_SECONDARYCOLOR();
}

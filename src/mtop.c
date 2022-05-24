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
#include "display/screen.h"
#include "display/tty.h"
#include "display/drawbuffer.h"
#include "process/proc.h"
#include "process/draw.h"
#include "xutil.h"
#include "opt.h"
#include "error.h"

procs_info_t info;
screensize_t ssz;

// arg parsing

/**
 * Parse command line arguments and set corresponding options.
 * If bad args are provided, this function will call the appropriate 
 * function and exit the program.
 * */
void parse_args(int argc, char** argv) {
	int n = argc;

	char** args = argv;

#define ARG_EQ_SL(SHORT, LONG) (x_streq(*args, SHORT) || x_streq(*args, LONG))
#define ARG_EQ(VALUE) (x_streq(*args, VALUE))
#define ARG_SHIFT() {args++; n--;}

	while (n) {
		if (ARG_EQ_SL("-r", "--refresh-rate")) {

			ARG_SHIFT();
			u32 r = atol(*args);
			if (r == 0) error(-1, "invalid value for field -r.");
			SET_OPT(refresh_rate, r);

		} else if (ARG_EQ_SL("-c", "--color")) {
			ARG_SHIFT();

			/* if (ARG_EQ("auto")) {
				; // TODO: automatically infer maximum possible colormode,
				  // using available capabilities (termcap, etc.)
			} else */

			if (ARG_EQ("none")) {
				SET_OPT(colormode, OPT_DRAWCOLOR_NONE);
			} else if (ARG_EQ("ansi")) {
				SET_OPT(colormode, OPT_DRAWCOLOR_ANSI);
			} else if (ARG_EQ_SL("8bit", "256")) {
				SET_OPT(colormode, OPT_DRAWCOLOR_8BIT);
			} else if (ARG_EQ("24bit")) {
				SET_OPT(colormode, OPT_DRAWCOLOR_24BIT);
			} else {
				error(-1, "invalid argument for color.");
			}

		} else if (ARG_EQ_SL("-s", "--static")) {
			SET_OPT(draw_static, OPT_YES);
		} else {
			error(-1, "invalid option '%s'. Type --help for more info.", *args);
		}

		ARG_SHIFT();
	}
}

// process list related functions

/**
 * Updates procs list and renews all drawcaches
 * */
void update_procs() {
	procs_update(&info);
	procs_foreachnode(&info, &pd_updatecache);
}

/** 
 * Initializes and populates procs_info struct.
 * Sets the selected cursor to the last process in the list.
 * */
void fill_procs() {
	info = procs_init();
	update_procs();
	procs_select(&info, PROCS_SELECT_LAST);
}

/**
 * randomizes draw values for all processes in the proclist.
 * pretty self explanatory. 
 * Note: these values are pretty conservative 
 * XXX: replace these with values that correspond to the 
 * CPU load of the process?
 * */ 

void randomize_drawvalue(procnode_t* node) {
	pd_random_drawctx(&node->dd.ctx);
	node->dd.offset = rand() % 20;
}

void randomize_drawvalues() {
	procs_foreachnode(&info, randomize_drawvalue);
}

void advance_offset(procnode_t* p) {
	//p->drawdata.offset += 1;
	//p->drawdata.offset += ((p->pid) % 2);
	pd_retract_drawctx(&p->dd.ctx);
}

// signals and terminating functions

void forceful_exit() {
	exit(1);
}

void graceful_exit(const char* msg) {
	procs_destroy(&info);
	screen_exit();
	if (msg != NULL) printf("%s\n", msg);
	exit(0);
}

void abnormal_exit(const char* msg) {
	procs_destroy(&info);
	screen_exit();
	if (msg != NULL) printf("%s\n", msg);
	exit(0);
}

void sigint_handler() {
	signal(SIGINT, &forceful_exit);
	abnormal_exit("received INT signal, exiting.");
}

void sigterm_handler() {
	graceful_exit("received TERM signal, exiting.");
}

void sigwinch_handler() {
	// update stored screen size and do a "hard" reset
	// of the screen (erase everything and wati for the next write)
	//
	// TODO: update with better logic to draw a new screen 
	// immediately instead of waiting for the loop to come around 
	// again

	ssz = get_screensize();
	screen_setcursor((rowcol_t) {0,0});
	procs_set_drawopts(&info, info.step, ssz.rows, ssz.cols);
	screen_clear();
}

void segfault() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(1);
}

// DRAWING


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
					cchar = pd_ccharat(cur.current, r + info->row_offset);
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
    }

	// draw info window (this contains details for
	// the selected process
    if (info->open_windows & PROCS_WINDOW_PROCINFO) {
        SET_SECONDARYCOLOR();
        DRAW_HORIZONTAL_SEP();
        for (size_t i = 0; i < selected_info_winsz; i++) {
            x_memset(buf, ' ', info->display_size);
            pd_drawinfo(&pl_cur_at(&info->selected)->value, buf, info->display_size, i);
            dbuf_addsn(dbuf, buf, info->display_size);
        }
    }

	#undef SET_PRIMARY_COLOR
	#undef SET_SECONDARY_COLOR
	#undef DRAW_HORIZONTAL_SEP
}

// MAIN

int cmtop(int argc, char** argv) {

	opt_default();
	parse_args(argc - 1, argv + 1);
	opt_print();

	signal(SIGWINCH, &sigwinch_handler);
	signal(SIGSEGV, &segfault);
	signal(SIGINT, &sigint_handler);
	signal(SIGTERM, &sigterm_handler);

	fill_procs();
	//procs_set_drawopts(&info, 2, ssz.rows, ssz.cols);
	randomize_drawvalues();

	screen_open();
	info.step = 2;
	sigwinch_handler();
	screen_hidecursor();

#define DO_SLEEP() usleep(opt.refresh_rate * 1000)

	drawbuffer_t dbuf = dbuf_init();

	tty_clear();

	u8 flushcount = 0;
	u8 flushbreak = 5;

	char ch  ='\0';

while (1) {

		ch = tty_readc();

		if (ch) {
			switch (ch) {
				case 'o':
					info.open_windows ^= PROCS_WINDOW_SYSINFO;
					break;
				case 'p':
					info.open_windows ^= PROCS_WINDOW_PROCINFO;
					break;
				case 'h':
					procs_select(&info, PROCS_SELECT_PREV);
					break;
				case 'l':
					procs_select(&info, PROCS_SELECT_NEXT);
					break;
				case 'j':
					info.row_offset += 1;
					break;
				case 'k':
					if (info.row_offset > 0) info.row_offset--;
					break;
				case 'g':
				case '^':
					procs_select(&info, PROCS_SELECT_FIRST);
					break;
				case 'G':
				case '$':
					procs_select(&info, PROCS_SELECT_LAST);
					break;
				case 'q':
					goto exit_main;
			}
		}

		if (flushcount == flushbreak) {
			tty_oflush();
			tty_iflush();
		}

		flushcount += 1; 
		flushcount %= flushbreak;

		screen_setcursor((rowcol_t) {0,0});

		update_procs();
		
		// draw procs info at current state and flush to screen
		draw_fillbuffer(&dbuf, &info, ssz.rows);
		dbuf_flush(&dbuf);

		procs_foreachnode(&info, &advance_offset);
		if (! ch) DO_SLEEP();
	}

	//while (! tty_readc()) ;
	
exit_main:

	screen_showcursor();
	graceful_exit("exiting normally.");

	return 0;
}

// debugging functions

void print_timedelta(timedelta_t td, const char *title) {
	if (title != NULL) printf("%s: ", title);
	printf("%llu\t%llu\t(d = %llu)\n", td.last, td.current, td.delta);
}

void print_cpuinfo(cpuinfo_t cpuinfo) {
	print_timedelta(cpuinfo.times.user, "user");
	print_timedelta(cpuinfo.times.nice, "nice");
	print_timedelta(cpuinfo.times.system, "system");
	print_timedelta(cpuinfo.times.idle, "idle");
	print_timedelta(cpuinfo.times.iowait, "iowait");
	print_timedelta(cpuinfo.times.irq, "irq");
	print_timedelta(cpuinfo.times.softirq, "softirq");
	print_timedelta(cpuinfo.times.steal, "steal");
	print_timedelta(cpuinfo.times.guest, "guest");
	print_timedelta(cpuinfo.times.guest_nice, "guest_nice");
	printf("\n");
}

int test_matrix_lines() {
	rand_drawctx_t ctx;
	ctx.index = 0;
	ctx.offset = 6;
	ctx.visible = 1;
	ctx.rand = 0;
	ctx.hashdata.base = 1;
	ctx.hashdata.salt = 1;
	ctx.rand = pd_get_interval(ctx.hashdata, ctx.index);

	int num_lines = 10;
	char* lines_str = getenv("COLUMNS");
	num_lines = lines_str ? atoi(lines_str) : 100;

	while (1) {
		for (int i = 0; i < num_lines; i++) {
			putchar(randd_visible(ctx, i) ? '#' : '.');
		}

		printf("\t%lu %lu %d %d\n", ctx.index, ctx.offset, ctx.rand, ctx.visible);
		pd_advance_drawctx(&ctx);
		///sleep(1);
		usleep(10000);
	}

	return 0;
}

int test_drawbuffer() {
	signal(SIGINT, &sigint_handler);
	opt_default();
	screen_open();
	drawbuffer_t dbuf = dbuf_init();
	drawbuffer_t *d = &dbuf;
	dbuf_addcolor(d, BASE_COLOR);
	dbuf_adds(d, "this is a string of stuff");
	dbuf_addcolor(d, HIGHLIGHT_COLOR);
	dbuf_adds(d, " so don't get it twisted");
	dbuf_addcolor(d, BASE_COLOR);
	dbuf_adds(d, " you stinky fella");

	size_t n = 2048;
	char dest[n];
	size_t written = dbuf_renderto(d, dest, n);
	tty_writesn(dest, written);
	sleep(100);
	screen_exit();
	return 0;
}

int main(int argc, char** argv) {
	return cmtop(argc, argv);
	//return test_drawbuffer();
	//return test_matrix_lines();
}

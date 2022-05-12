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
#include <signal.h>
#include "screen.h"
#include "tty.h"
#include "proc.h"
#include "proclist.h"
#include "procdraw.h"
#include "draw.h"
#include "xutil.h"
#include "drawbuffer.h"
#include "dmeter.h"
#include "opt.h"
#include "error.h"

procs_info_t info;
screensize_t ssz;

#ifdef MTOP_DRAW_COLOR
color_t current_color;
#endif

// ARG PARSING

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

/**
 * Updates procs list and renews all drawcaches
 * */
void update_procs() {
	procs_update(&info);
	proclist_foreach(&info.procs, &pd_updatecache);
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
void randomize_drawvalues() {
	proclist_cur_t cur = pl_cur_init(&info.procs);
	pl_cur_next(&cur);
	while (cur.current != NULL) {
		procinfo_t* proc = &cur.current->value;
		pd_random_drawctx(&proc->drawdata.ctx);
		proc->drawdata.offset = rand() % 20;
		pl_cur_next(&cur);
	}
}

void advance_offset(procinfo_t* p) {
	//p->drawdata.offset += 1;
	//p->drawdata.offset += ((p->pid) % 2);
	pd_retract_drawctx(&p->drawdata.ctx);
}
#ifdef MTOP_DRAW_COLOR
void write_currentcolor() {
	char buf[20];
	x_memset(buf, '\0', 20);
	int w = draw_color(current_color, buf, 20);
	tty_writesn(buf, w);
}

void set_color(color_t color) {
	current_color = color;
}
#endif

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
#ifdef MTOP_DRAW_COLOR
	write_currentcolor();
#endif
}

void forceful_exit() {
	exit(1);
}

void graceful_exit() {
	procs_destroy(&info);
	screen_exit();
	printf("exiting.\n");
	exit(0);
}

void sigint_handler() {
	signal(SIGINT, &forceful_exit);
	graceful_exit();
}

void sigterm_handler() {
	signal(SIGTERM, &forceful_exit);
	graceful_exit();
}

void segfault() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(1);
}

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

#ifdef MTOP_DRAW_COLOR
	//tty_writes("\e[32;1m");
	//tty_writes("\e[38;2;20;220;20m");
	/*
	color_t color;
	color.rgb = (rgb_t) {40,240,40};
	color.hue = COLOR_GREEN;
	color.stage = COLOR_FG;
	color.nature = COLOR_BRIGHT;
	set_color(color);
	write_currentcolor();
	*/
#endif

	drawbuffer_t dbuf = dbuf_init();

	tty_clear();

	u8 flushcount = 0;
	u8 flushbreak = 5;

	char ch  ='\0';
	u8 quit_now = 0;

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
					quit_now = 1;
					break;
			}
		}

		if (quit_now) break;

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

		proclist_foreach(&info.procs, &advance_offset);
		if (! ch) DO_SLEEP();
	}

	//while (! tty_readc()) ;

	screen_showcursor();
	graceful_exit();

	return 0;
}

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
	screen_open();
	drawbuffer_t dbuf = dbuf_init();
	drawbuffer_t *d = &dbuf;
	dbuf_addcolor(d, BASE_COLOR);
	dbuf_adds(d, "hello");
	dbuf_addcolor(d, HIGHLIGHT_COLOR);
	dbuf_adds(d, " there you!");
	dbuf_addcolor(d, BASE_COLOR);
	dbuf_adds(d, "this is a string of stuff");
	dbuf_adds(d, " so don't get it twisted");
	dbuf_draw(d);
	sleep(1);
	screen_exit();
	return 0;
}

int main(int argc, char** argv) {
	return cmtop(argc, argv);
	//return test_drawbuffer();
	//return test_matrix_lines();
}

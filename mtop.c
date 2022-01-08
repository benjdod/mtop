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

procs_info_t info;
screensize_t ssz;
char* scrbuf = NULL;

#ifdef MTOP_DRAW_COLOR
color_t current_color;
#endif

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
		cur.current->value.drawdata.offset = rand() % 50;
		cur.current->value.drawdata.padding = 10 + (rand() % 6);
		pl_cur_next(&cur);
	}
}

void advance_offset(procinfo_t* p) {
	p->drawdata.offset += 1;
	p->drawdata.offset += ((p->pid) % 2);
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
	ssz = get_screensize();
	x_free(scrbuf);
	scrbuf = (char*) x_malloc(ssz.rows * ssz.cols, sizeof(char));
	screen_setcursor((rowcol_t) {0,0});
	tty_clear();
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

void segfault() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(1);
}

int cmtop() {

	opt_init();

	signal(SIGWINCH, &sigwinch_handler);
	signal(SIGSEGV, &segfault);
	signal(SIGINT, &sigint_handler);

	fill_procs();
	randomize_drawvalues();

	screen_open();
	sigwinch_handler();
	screen_hidecursor();

	int sleeptime = 50 * 1000;

#define DO_SLEEP() usleep(sleeptime)

#ifdef MTOP_DRAW_COLOR
	draw_setopts((DRAW_COLOR|DRAW_RGBCOLOR));
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

	while (1) {

		char ch = tty_readc();

		if (ch) {
			if (ch == 'j') {
				procs_select(&info, PROCS_SELECT_PREV);
			} else if (ch == 'k') {
				procs_select(&info, PROCS_SELECT_NEXT);
			} else if (ch == 'q') break;
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
		draw_fillbuffer(&dbuf, &info, ssz.rows, ssz.cols);
		dbuf_flush(&dbuf);

		proclist_foreach(&info.procs, &advance_offset);
		DO_SLEEP();
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
	print_timedelta(cpuinfo.total.user, "user");
	print_timedelta(cpuinfo.total.nice, "nice");
	print_timedelta(cpuinfo.total.system, "system");
	print_timedelta(cpuinfo.total.idle, "idle");
	print_timedelta(cpuinfo.total.iowait, "iowait");
	print_timedelta(cpuinfo.total.irq, "irq");
	print_timedelta(cpuinfo.total.softirq, "softirq");
	print_timedelta(cpuinfo.total.steal, "steal");
	print_timedelta(cpuinfo.total.guest, "guest");
	print_timedelta(cpuinfo.total.guest_nice, "guest_nice");
	printf("\n");
}

int testlist() {

	proclist_t list = proclist_init();

	pid_t pids[] = {
		1,
		3,
		5,
		17
	};

	u16 num_pids = sizeof(pids) / sizeof(pid_t);

	for (int i = 0; i < num_pids; i++) {
		procinfo_t n;
		n.pid = pids[i];
		proclist_insert(&list, n);
	}

	proclist_cur_t cur = pl_cur_init(&list);

	pl_cur_next(&cur);

	while (pl_cur_at(&cur) != NULL) {
		printf("%d\n", pl_cur_at(&cur)->pid);
		pl_cur_next(&cur);
	}

	proclist_destroy(&list);

	return 0;
}

int main() {
	return cmtop();
	//return testlist();
}

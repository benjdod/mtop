#include "common.h"
#include <signal.h>
#include "screen.h"
#include "tty.h"
#include "proc.h"
#include "procdraw.h"
#include "draw.h"
#include "xutil.h"
#include "drawbuffer.h"
#include "dmeter.h"
#include "opt.h"

procs_info_t info;
screensize_t ssz;
char* scrbuf = NULL;

#ifdef CMTOP_DRAW_COLOR
color_t current_color;
#endif

void update_procs() {
	procs_update(&info);
	procbst_inorder(&info.procs, &pd_updatecache);
}

void fill_procs() {
	info = procs_init();
	update_procs();
}

void randomize_drawvalues() {
	procbst_cursor_t cur = procbst_cursor_init(&info.procs);
	procbst_cursor_next(&cur);
	cur.current->value.drawdata.offset = 5;

	while (cur.current != NULL) {
		cur.current->value.drawdata.offset = rand() % 50;
		cur.current->value.drawdata.padding = 10 + (rand() % 6);
		procbst_cursor_next(&cur);
	}
}

void advance_offset(procinfo_t* p) {
	p->drawdata.offset += 1;
	p->drawdata.offset += ((p->pid) % 2);
}

#ifdef CMTOP_DRAW_COLOR
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
#ifdef CMTOP_DRAW_COLOR
	write_currentcolor();
#endif
}


void forceful_exit() {
	exit(1);
}

void graceful_exit() {
	procbst_destroy(&info.procs);
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

#ifdef CMTOP_DRAW_COLOR
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
			if (ch == 'j') procbst_cursor_prev(&info.selected);
			else if (ch == 'k') procbst_cursor_next(&info.selected);
			else if (ch == 'q') break;
		}

		if (flushcount == flushbreak) {
			tty_oflush();
			tty_iflush();
		}

		// if (flushcount)break;

		flushcount += 1; 
		flushcount %= flushbreak;

		screen_setcursor((rowcol_t) {0,0});

		update_procs();

		//draw_queryscr(&info, scrbuf, ssz.rows, ssz.cols, 0, 0, 3);
		//tty_writesn(scrbuf, (int) (ssz.rows * ssz.cols));

		draw_fillbuffer(&dbuf, &info, ssz.rows, ssz.cols);
		dbuf_flush(&dbuf);

		procbst_inorder(&info.procs, &advance_offset);
		DO_SLEEP();
	}

	//while (! tty_readc()) ;

	screen_showcursor();
	graceful_exit();
}

void print_timedelta(timedelta_t td, const char *title) {
	if (title != NULL) printf("%s: ", title);
	printf("%llu\t%llu\t(d = %llu)\n", td.last, td.current, td.delta);
}

void print_cpuinfo(sys_cpuinfo_t cpuinfo) {
	print_timedelta(cpuinfo.user, "user");
	print_timedelta(cpuinfo.nice, "nice");
	print_timedelta(cpuinfo.system, "system");
	print_timedelta(cpuinfo.idle, "idle");
	print_timedelta(cpuinfo.iowait, "iowait");
	print_timedelta(cpuinfo.irq, "irq");
	print_timedelta(cpuinfo.softirq, "softirq");
	print_timedelta(cpuinfo.steal, "steal");
	print_timedelta(cpuinfo.guest, "guest");
	print_timedelta(cpuinfo.guest_nice, "guest_nice");
	printf("\n");
}

int meter() {
	dmeter_t dm;
	dm.min = 0;
	dm.max = 100;
	dm.value = 25;
	dm.width = 50;

	char buf[58];

	while(1) {

		dm.value = rand() % 100;

		x_memset(buf, '\0', 58);
		dmeter_draw(dm, buf, 57);

		printf("meter: %s", buf);
		usleep(10 * 1000);
		printf("\r");
	}


	return 0;
}

int main() {
	return cmtop();
	//return meter();
}

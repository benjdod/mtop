#include "common.h"
#include <signal.h>
#include "screen.h"
#include "tty.h"
#include "proc.h"
#include "procdraw.h"
#include "draw.h"
#include "xutil.h"

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
	memset(buf, '\0', 20);
	int w = draw_color(current_color, buf, 20);
	tty_writesn(buf, w);
}

void set_color(color_t color) {
	current_color = color;
}
#endif

void sigwinch_handler() {
	ssz = get_screensize();
	free(scrbuf);
	scrbuf = (char*) malloc(ssz.rows * ssz.cols);
	screen_setcursor((rowcol_t) {0,0});
	tty_clear();
#ifdef CMTOP_DRAW_COLOR
	write_currentcolor();
#endif
}

void graceful_exit() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(0);
}

void segfault() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(1);
}


int cmtop() {

	signal(SIGWINCH, &sigwinch_handler);
	signal(SIGSEGV, &segfault);
	signal(SIGINT, &graceful_exit);

	fill_procs();
	randomize_drawvalues();

	screen_init();
	sigwinch_handler();
	screen_hidecursor();

#ifdef CMTOP_DRAW_COLOR
	draw_setopts((DRAW_COLOR|DRAW_RGBCOLOR));
	//tty_writes("\e[32;1m");
	//tty_writes("\e[38;2;20;220;20m");
	color_t color;
	color.rgb = (rgb_t) {40,240,40};
	color.hue = COLOR_GREEN;
	color.stage = COLOR_FG;
	color.nature = COLOR_BRIGHT;
	set_color(color);
	write_currentcolor();
#endif


	tty_clear();

	u8 flushcount = 0;
	u8 flushbreak = 5;

	while (1) {

		if (flushcount == flushbreak) {
			tty_oflush();
			tty_iflush();
		}

		flushcount = (flushcount++) % flushbreak;

		update_procs();

		draw_queryscr(&info, scrbuf, ssz.rows, ssz.cols, 0, 0, 3);
		tty_writesn(scrbuf, (int) (ssz.rows * ssz.cols));
		if (tty_readc()) break;
		procbst_inorder(&info.procs, &advance_offset);
		usleep(1000*1000);
	}

	screen_exit();
	screen_showcursor();
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

int get_proc() {

	info = procs_init();

	while (1) {
		procs_update(&info);
		//procbst_inorder(&info.procs, &update_drawcache);
		//print_cpuinfo(info.cpuinfo);

		usleep(1000 * 1000);
	}
	
	return 0;
}

int main() {
	return cmtop();
}

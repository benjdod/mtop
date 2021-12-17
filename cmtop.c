#include <stdio.h>
#include "common.h"
#include <signal.h>
#include "screen.h"
#include "tty.h"
#include "proc.h"
#include "procdraw.h"
#include "draw.h"

procs_info_t info;
screensize_t ssz;

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
		cur.current->value.drawdata.offset = rand() % 25;
		cur.current->value.drawdata.padding = 15 + (rand() % 10);
		procbst_cursor_next(&cur);
	}
}

void advance_offset(procinfo_t* p) {
	p->drawdata.offset += 1;
}

void sigwinch_handler() {
	ssz = get_screensize();
}

void graceful_exit() {
	screen_exit();
	printf("program encountered a segmentation fault, exiting.\n");
	exit(0);
}

int cmtop() {

	signal(SIGWINCH, &sigwinch_handler);
	signal(SIGSEGV, &graceful_exit);

	fill_procs();
	randomize_drawvalues();

	screen_init();

	ssz = get_screensize();
	screen_hidecursor();

	char wbuf[ssz.cols];

#ifdef CMTOP_DRAW_COLOR
	tty_writes("\e[32;1m");
#endif

	while (1) {

		update_procs();
		
		//tty_clear();
		screen_setcursor((rowcol_t) {0,0});
		for (int i = 0; i < ssz.rows; i++) {
			memset(wbuf, ' ', ssz.cols);
			size_t written = draw_queryrow(&info, wbuf, (size_t) ssz.cols, i, 0, 2);
			tty_writesn(wbuf, written);
			tty_fill(' ', (ssz.cols - written));
			//tty_writed(written);

			//tty_writesn("\r\n", 2);
		}
		if (tty_readc()) break;
		procbst_inorder(&info.procs, &advance_offset);
		usleep(100*1000);
	}

	screen_exit();
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
	//return cmtop();

	cmtop();

	return 0;
}

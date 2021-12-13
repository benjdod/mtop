#include <stdio.h>
#include "screen.h"
#include "tty.h"
#include "mtxline.h"
#include "proc.h"

int cmtop() {
	screen_init();

	screensize_t ssz = get_screensize();
	screen_hidecursor();

	mtx_init_lines(ssz.cols, ssz.rows);

	char *buf = malloc(ssz.cols * ssz.rows * sizeof(char));

	while (1) {
		if (tty_readc()) break;
//		tty_clear();
		screen_setcursor((rowcol_t) {0,0});
		for (size_t i = 0; i < ssz.rows; i++) {
			mtx_query_row(i, buf + i * ssz.cols, ssz.cols);
		}
		tty_writesn(buf, ssz.rows * ssz.cols);
		mtx_step_lines();
		usleep(100 * 1000);
	}

	screen_exit();
}

void print_procinfo(procinfo_t p) {
	printf("(%s) - %s\t%d\t%c\t", p.user, p.cmd, p.pid, p.state);
	printf("time: %llu\n", p.cpuavg.ttime.current);
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

	procs_info_t info = procs_init();

	while (1) {
		procs_update(&info);
		//procbst_inorder(&info.procs, &print_procinfo);
		print_cpuinfo(info.cpuinfo);

		usleep(1000 * 1000);
	}

	
	return 0;
}


int main() {
	return get_proc();
}

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
			mtx_query_row(i, buf + i * ssz.cols, ssz.cols);;
		}
		tty_writesn(buf, ssz.rows * ssz.cols);
		mtx_step_lines();
		usleep(100 * 1000);
	}

	screen_exit();
}

void print_procinfo(procinfo_t p) {
	printf("(%s) - %s\t%d\n", p.user, p.cmd, p.pid);
}

int get_proc() {

	procs_info_t info = procs_init();
	procs_update(&info);

	procbst_inorder(&info.procs, &print_procinfo);

	return 0;
}


int main() {
	return get_proc();
}

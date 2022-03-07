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
#include "error.h"
#include "proclist.h"
#include "proc.h"

#define PROCINFO_BUFSZ 8192
#define CPU_PERIOD_EPS 1E-6
#define I_EPS(val, eps) (((val) < (eps)) ? 0 : (val))
#define F_EPS(val, eps) (((val) < (eps)) ? 0.0F : (val))

char* null_cmd = "(nil)";

static long jiffy = 1;
static long pagesize = 1;

#define PROCINFO_FOUND 0x00001

static inline ptime_t adjust_time(ptime_t time) {
	// this function probably won't do anything if the 
	// jiffy value is set to hundredths of a second 
	// (likely on modern linux kernels...)
	return time * 100 / jiffy;
}

static char* strip_pathinfo(char* cmd_path) {
	char* current = cmd_path;

	size_t last_fslash = 0;

	for (size_t i = 0; *current != '\0'; i++, current++) {
		if (*current == '/') last_fslash = i;
	}

	return (char*) (cmd_path + last_fslash);
}

static cmdline_args_t make_cmdline_args(char** argp) {
	cmdline_args_t args;
	int arg_idx = 0;
	int num_args = 0;

	args.argc = 0;
	args.argv = NULL;

	if (argp == NULL) {
		return args;
	}

	while (argp[arg_idx] != NULL && argp[arg_idx][0] != '\0') {
		arg_idx++; num_args++;
	}

	args.argc = num_args;
	args.argv = x_cvalloc(num_args, sizeof(char*), (long) NULL);

	for (int i = 0; i < num_args; i++) {
		size_t arg_length = x_strlen(argp[i]);
		args.argv[i] = x_malloc(arg_length + 1, sizeof(char));
		x_strncpy(args.argv[i], argp[i], arg_length);
		args.argv[i][arg_length] = '\0';
	}

	return args;

	/*
	while (1) {
		printf("%s\n", p);
		while (*p != '\0') {
			p++;
		}
		p++;

		if (*p == '\0') break;
	}
	*/
}

static void destroy_cmdline_args(cmdline_args_t* argp) {
	for (int i = 0; i < argp->argc; i++) {
		argp->argv[i] = x_free(argp->argv[i]);
	}
	argp->argc = 0;
}

const char* proc_state_tostring(char state) {

	// https://man7.org/linux/man-pages/man5/proc.5.html > /proc/[pid]/stat > state

	switch (state) {
		case 'R':
			return "running";
		case 'S':
			return "sleeping";
		case 'D':
			return "waiting";
		case 'Z':
			return "zombie";
		case 'T':
			return "stopped";
		case 't':	// > 2.6.33
			return "tracing stop";
		case 'W':
			return "waking"; 	// 2.6.33 - 3.13
			// return "paging" for linux < 2.6.0

		case 'X':	// > 2.6.0
		case 'x':	// 2.6.33 - 3.13
			return "dead";
		case 'K':	// 2.6.33 - 3.13
			return "wakekill";
		case 'P':	// 3.9 - 3.13
			return "parked";
		default:
			return "";
	}
}

size_t proc_state_getstring(char state, char* buf) {
	return strlen(x_strncpy(buf, proc_state_tostring(state), 13));
}

static cpuinfo_t cpuinfo_init() {
	cpuinfo_t info;
	x_memset(&info, 0, sizeof(cpuinfo_t));
	info.num_cores = (u16) sysconf(_SC_NPROCESSORS_ONLN);
	info.coretimes = x_malloc(info.num_cores, sizeof(cputimes_t));
	return info;
}

static meminfo_t meminfo_init() {
	meminfo_t out;
	x_memset(&out, 0, sizeof(meminfo_t));
	return out;
}

void cpuinfo_destroy(cpuinfo_t* info) {
	info->coretimes = x_free(info->coretimes);
}

procs_info_t procs_init() {
	sysinfo_t sys;
	sys.num_procs = 0;
	sys.running = 0;
	sys.cpu = cpuinfo_init();
	sys.mem = meminfo_init();

	procs_info_t pi;

	pi.procs = proclist_init();
	pi.refresh_rate = 1000 * 100;
	pi.selected = pl_cur_init(&pi.procs);
	pi.selected_index = 0;

	pi.sys = sys;

	jiffy = sysconf(_SC_CLK_TCK);
	pagesize = sysconf(_SC_PAGESIZE);
	return pi;
}

#ifdef MTOP_PROC_DRAW
void procs_set_drawopts(procs_info_t* info, size_t step, size_t rsize, size_t csize) {
	// info->selected_index = 0;
	info->col_offset = 0;
	info->row_offset = 0;
	info->display_size = csize;
	info->step = step;
	info->real_size = csize / info->step + (csize % info->step ? 1 : 0);
}
#endif

size_t procs_select(procs_info_t* info, u8 select) {

	// initialize the cursor if needed in a sensible way
	if (info->selected.list == NULL) {
		if (select == PROCS_SELECT_NEXT) select = PROCS_SELECT_FIRST;
		if (select == PROCS_SELECT_PREV) select = PROCS_SELECT_LAST;
	}

	switch ( select ) {
		case PROCS_SELECT_FIRST:
			info->selected = pl_cur_init(&info->procs);
			pl_cur_first(&info->selected);
			info->selected_index = 0;
			break;
		case PROCS_SELECT_LAST:
			info->selected = pl_cur_init(&info->procs);
			pl_cur_last(&info->selected);
			info->selected_index = info->sys.num_procs - 1;
			break;

		// for next and prev we can assume that there is an
		// active cursor
		case PROCS_SELECT_NEXT:
			if (pl_cur_next(&info->selected) != NULL) info->selected_index += 1;
			else pl_cur_prev(&info->selected);
			break;
		case PROCS_SELECT_PREV:
			if (pl_cur_prev(&info->selected) != NULL) info->selected_index -= 1;
			else pl_cur_next(&info->selected);
			break;
	}

	return info->selected_index;
}

void procs_destroy(procs_info_t* procs) {
	proclist_destroy(&procs->procs);
	cpuinfo_destroy(&procs->sys.cpu);
}

static int read_cpuinfo(cpuinfo_t* info_ptr) {
	
#define CPUTIME_UPDATE(ts, user, nice, sys, idle, io, irq, softirq, steal, guest, guest_nice) {\
	TIMEDELTA_COND_UPDATE(ts.user, user); \
	TIMEDELTA_COND_UPDATE(ts.nice, nice); \
	TIMEDELTA_COND_UPDATE(ts.system, sys); \
	TIMEDELTA_COND_UPDATE(ts.idle, idle); \
	TIMEDELTA_COND_UPDATE(ts.iowait, io); \
	TIMEDELTA_COND_UPDATE(ts.irq, irq); \
	TIMEDELTA_COND_UPDATE(ts.softirq, softirq); \
	TIMEDELTA_COND_UPDATE(ts.steal, steal); \
	TIMEDELTA_COND_UPDATE(ts.guest, guest); \
	TIMEDELTA_COND_UPDATE(ts.guest_nice, guest_nice); \
	TIMEDELTA_COND_UPDATE(ts.total, user + nice + system + idle);\
}

#define CPUINFO_BUFSZ 1024	// should be enough

	char buf[CPUINFO_BUFSZ];
	x_memset(buf, '\0', CPUINFO_BUFSZ);
	x_readfile("/proc/stat", buf, CPUINFO_BUFSZ);

	// XXX: there is likely a better way to do this.

	char *valstart = buf + 3;

	while (*valstart == ' ') valstart++;

	ptime_t 
		user = 0, 
		nice = 0, 
		system = 0, 
		idle = 0, 
		iowait = 0, 
		irq = 0, 
		softirq = 0, 
		steal = 0, 
		guest = 0, 
		guest_nice = 0;
		
	sscanf(valstart, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
		&user,
		&nice,
		&system,
		&idle,
		&iowait,
		&irq,
		&softirq, 
		&steal,
		&guest,
		&guest_nice
	);

	// TODO: add time super groups or generator functions 
	// there seem to be 3 meaningful time sums as written here:
	// https://gitlab.com/procps-ng/procps/-/blob/newlib/proc/stat.h : 60
/*
	TIMEDELTA_COND_UPDATE(info_ptr->total.user, user);
	TIMEDELTA_COND_UPDATE(info_ptr->total.nice, nice);
	TIMEDELTA_COND_UPDATE(info_ptr->total.system, system);
	TIMEDELTA_COND_UPDATE(info_ptr->total.idle, idle);
	TIMEDELTA_COND_UPDATE(info_ptr->total.iowait, iowait);
	TIMEDELTA_COND_UPDATE(info_ptr->total.irq, irq);
	TIMEDELTA_COND_UPDATE(info_ptr->total.softirq, softirq);
	TIMEDELTA_COND_UPDATE(info_ptr->total.steal, steal);
	TIMEDELTA_COND_UPDATE(info_ptr->total.guest, guest);
	TIMEDELTA_COND_UPDATE(info_ptr->total.guest_nice, guest_nice);
	TIMEDELTA_COND_UPDATE(info_ptr->total.total, user + nice + system + idle);*/

	CPUTIME_UPDATE(info_ptr->times, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice);

#define SEEK_NEXTLINE(ptr) {while (*ptr != '\n' && *ptr != '\0') ptr++; if (*ptr == '\n') ptr++;}

	char *p = buf;

	for (u16 i = 0; i < info_ptr->num_cores; i++) {
		SEEK_NEXTLINE(p);

		// skip "cpu<no.>   "

		p += 3;					// skip "cpu"
		while (*p != ' ') p++;	// skip <no>
		while (*p == ' ') p++;	// skip ' '

		sscanf(valstart, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", 
			&user,
			&nice,
			&system,
			&idle,
			&iowait,
			&irq,
			&softirq, 
			&steal,
			&guest,
			&guest_nice
		);

		CPUTIME_UPDATE(info_ptr->coretimes[i], user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice);
	}

#undef CPUINFO_BUFSZ

	return 0;
}

static int read_meminfo(meminfo_t* info_ptr) {
#define MEMINFO_BUFSZ 1024
	FILE* proc_mem = fopen("/proc/meminfo", "r");
	char buf[MEMINFO_BUFSZ];


	x_memset(buf, '\0', MEMINFO_BUFSZ);
	while ((fgets(buf, MEMINFO_BUFSZ - 1, proc_mem)) != 0) {
		char* value = buf;
		while (*value != ' ') value++;
		while (*value == ' ') value++;
#define FILL_LU_VALUE(field, searchstring) \
	if (x_strstr(buf, #searchstring) != NULL) { \
 		sscanf(value, "%lu", &info_ptr->field); \
	} else 

		FILL_LU_VALUE(total, MemTotal)
		FILL_LU_VALUE(free, MemFree)
		FILL_LU_VALUE(available, MemAvailable)
		FILL_LU_VALUE(swap_total, SwapTotal)
		FILL_LU_VALUE(swap_free, SwapFree)
		{ // final else clause to satisfy the macro
		  	; 
		}
	}

	return 0;
}

void proc_updateinfo(procinfo_t* p, proc_t proc, ptime_t period) {

	// update all the non-constant fields 
	// (everything except pid, user, cmd, stuff like that)...

	p->priority 	= proc.priority;
	p->nice 		= proc.nice;

	// memory data is given as # of pages,
	// so to get bytes we have to multiply to
	// get a useful number
	p->virt_mem		= proc.size * pagesize / 1024;
	p->res_mem		= proc.resident * pagesize / 1024;
	p->shr_mem		= proc.share * pagesize / 1024;

	p->state		= proc.state;
	p->cpu_pct		= 0;
	p->mem_pct		= 0;
	p->start_time	= proc.start_time;

	TIMEDELTA_UPDATE(p->cpuavg.stime, adjust_time(proc.stime));
	TIMEDELTA_UPDATE(p->cpuavg.utime, adjust_time(proc.utime));
	TIMEDELTA_UPDATE(p->cpuavg.cutime, adjust_time(proc.cutime));
	TIMEDELTA_UPDATE(p->cpuavg.cstime, adjust_time(proc.cstime));
	TIMEDELTA_UPDATE(p->cpuavg.ttime, adjust_time(proc.stime + proc.utime));

	// from htop:
	// float percent_cpu = (period < 1E-6) ? 0.0F : ((lp->utime + lp->stime - lasttimes) / period * 100.0);

	timedelta_t cputotal = p->cpuavg.ttime;

	if (period > CPU_PERIOD_EPS) {
		float percent_cpu = cputotal.delta * 100.0F / period;
		p->cpu_pct = percent_cpu;
	}

	//proc_cpuavg_t cpuavg = p->cpuavg;

	//printf("proc %d (%s): ", p->pid, p->cmd);
	//printf("period: %llu \tproc period: %llu\tcpu_pct: %f\n", period, cputotal.delta, p->cpu_pct);


	return;
}

static void proc_touch(procinfo_t* p) {
	p->flags |= PROCINFO_FOUND;
}

static void proc_untouch(procinfo_t* p) {
	p->flags &= ~(PROCINFO_FOUND);
}

size_t procs_update(procs_info_t *info) {

	size_t num_procs = 0;
	size_t running_procs = 0;

	read_cpuinfo(&info->sys.cpu);
	read_meminfo(&info->sys.mem);

	PROCTAB* processes = openproc(
			PROC_FILLMEM | 
			PROC_FILLSTAT | 
			PROC_FILLSTATUS | 
			PROC_FILLUSR | 
			PROC_FILLCOM
		);

	proc_t *proc;

	while ((proc = readproc(processes, NULL)) != NULL)  {

		procinfo_t* proc_ptr = NULL;

		ptime_t period = info->sys.cpu.times.total.delta;

		if ((proc_ptr = proclist_find(&info->procs, PROC_PIDOF(*proc))) != NULL) {
			proc_updateinfo(proc_ptr, *proc, period);
		} else {
			proc_ptr = proclist_insert(&info->procs, proc_getinfo(*proc, period));
		}

		proc_touch(proc_ptr);	// mark that the process in the tree has been found this round
		freeproc(proc);
		num_procs++;
		if (proc->state == 'R' || proc->state == 'r') running_procs += 1;
	}

	closeproc(processes);


	// iterate over all processes in list, deleting processes that were 
	// not listed in the read of the latest PROCTAB

	proclist_cur_t cur = pl_cur_init(&info->procs);
	pl_cur_next(&cur);
	while (cur.current != NULL) {
		if (!(PL_CURVAL(&cur)->flags & PROCINFO_FOUND)) {

			if (pl_cur_eq(&cur, &info->selected)) {
				//pl_cur_prev(&info->selected);
				procs_select(info, PROCS_SELECT_PREV);
				info->selected_index--;
			}

			pl_cur_remove(&cur, PL_CURSHIFT_RIGHT);
			num_procs--;
		} else {
			proc_untouch(&cur.current->value);
			pl_cur_next(&cur);
		}
	}

	info->sys.num_procs = num_procs;
	info->sys.running = running_procs;
	
	// after everything has been updated, fill the selected
	// cursor if it's empty. Set it to the last entry by default
	
	if (info->selected.current == NULL) {
		info->selected = pl_cur_init(&info->procs);
		info->selected_index = procs_select(info, PROCS_SELECT_LAST);
	}

	return info->sys.num_procs;
}

procinfo_t proc_getinfo(proc_t proc, ptime_t period) {

	procinfo_t p;

	p.flags = 0;

#ifdef MTOP_PROC_DRAW
	drawdata_t d;
	d.offset = 0;
	x_memset(d.cache, '\0', sizeof(d.cache));
	p.drawdata = d;
	p.drawdata.offset = 0;
#endif

	// reference defn of proc_t:
	// https://github.com/thlorenz/procps/blob/master/deps/procps/proc/readproc.h
	
	p.pid 		= PROC_PIDOF(proc);

	const char *user_ptr = PROC_USEROF(proc);

	size_t userlen = strlen(user_ptr);
	p.user 		= (char*) x_calloc((userlen + 1), sizeof(char));
	p.user[userlen] = '\0';
	x_strncpy(p.user, user_ptr, userlen);

	const char *cmd_ptr;

	if (proc.cmdline != NULL) {
		cmd_ptr = strip_pathinfo(proc.cmd);
		//cmd_ptr = proc.cmdline[0];

		size_t cmd_len = strlen(cmd_ptr);
		p.cmd = x_calloc((cmd_len + 1), sizeof(char));
		x_strncpy(p.cmd, cmd_ptr, cmd_len);
	} else {
		p.cmd = null_cmd;
	}

	p.args = make_cmdline_args(proc.cmdline);

	proc_updateinfo(&p, proc, period);

	return p;
}

void proc_freeinfo(procinfo_t* p_info) {
	x_free(p_info->user);

	if (p_info->cmd != null_cmd) {
		x_free(p_info->cmd);
	}

	if (p_info->args.argc > 0) {
		destroy_cmdline_args(&p_info->args);
	}
}

void proc_dummymain() {
	procs_info_t procs = procs_init();
	procs_update(&procs);
}

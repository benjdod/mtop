#include "common.h"
#include "xutil.h"
#include "error.h"
#include "procbst.h"
#include "proc.h"

#define PROCINFO_BUFSZ 8192
#define CPU_PERIOD_EPS 1E-6
#define I_EPS(val, eps) (((val) < (eps)) ? 0 : (val))
#define F_EPS(val, eps) (((val) < (eps)) ? 0.0F : (val))

char* null_cmd = "\0";

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
	}
}

size_t proc_state_getstring(char state, char* buf) {
	x_strncpy(buf, proc_state_tostring(state), 13);
}


procs_info_t procs_init() {

	procs_info_t pi;

	pi.num_procs = 0;
	pi.procs = procbst_init();
	pi.refresh_rate = 1000 * 100;
	x_memset(&pi.cpuinfo, 0, sizeof(sys_cpuinfo_t));
	pi.selected = procbst_cursor_init(&pi.procs);

	jiffy = sysconf(_SC_CLK_TCK);
	pagesize = sysconf(_SC_PAGESIZE);
	//printf("got jiffy %ld\n", jiffy);

	return pi;
}

int read_cpuinfo(sys_cpuinfo_t *info_ptr) {

#define CPUINFO_BUFSZ 256

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

	TIMEDELTA_COND_UPDATE(info_ptr->user, user);
	TIMEDELTA_COND_UPDATE(info_ptr->nice, nice);
	TIMEDELTA_COND_UPDATE(info_ptr->system, system);
	TIMEDELTA_COND_UPDATE(info_ptr->idle, idle);
	TIMEDELTA_COND_UPDATE(info_ptr->iowait, iowait);
	TIMEDELTA_COND_UPDATE(info_ptr->irq, irq);
	TIMEDELTA_COND_UPDATE(info_ptr->softirq, softirq);
	TIMEDELTA_COND_UPDATE(info_ptr->steal, steal);
	TIMEDELTA_COND_UPDATE(info_ptr->guest, guest);
	TIMEDELTA_COND_UPDATE(info_ptr->guest_nice, guest_nice);
	TIMEDELTA_COND_UPDATE(info_ptr->total, user + nice + system + idle);

#undef CPUINFO_BUFSZ
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
	ptime_t cputotal_delta = cputotal.delta;

	if (period > CPU_PERIOD_EPS) {
		float percent_cpu = cputotal.delta * 100.0F / period;
		p->cpu_pct = percent_cpu;
	}

	proc_cpuavg_t cpuavg = p->cpuavg;

	//printf("proc %d (%s): ", p->pid, p->cmd);
	//printf("period: %llu \tproc period: %llu\tcpu_pct: %f\n", period, cputotal.delta, p->cpu_pct);


	return;
}

static void proc_touch(procinfo_t* p) {
	p->flags |= PROCINFO_FOUND;
}

size_t procs_update(procs_info_t *info) {

	size_t num_procs;

	read_cpuinfo(&info->cpuinfo);

	PROCTAB* processes = openproc(
			PROC_FILLMEM | 
			PROC_FILLSTAT | 
			PROC_FILLSTATUS | 
			PROC_FILLUSR | 
			PROC_FILLCOM		// allocated! must be freed
		);

	proc_t *proc;
	//x_memset(&proc, 0, sizeof(proc));

	while ((proc = readproc(processes, NULL)) != NULL)  {

		procinfo_t* proc_ptr = NULL;

		ptime_t period = info->cpuinfo.total.delta;

		if ((proc_ptr = procbst_find(&info->procs, PROC_PIDOF(*proc))) != NULL) {
			proc_updateinfo(proc_ptr, *proc, period);
		} else {
			proc_ptr = procbst_insert(&info->procs, proc_getinfo(*proc, period));
		}

		proc_touch(proc_ptr);	// mark that the process in the tree has been found this round
		//x_memset(&proc, 0, sizeof(proc));

		freeproc(proc);
		num_procs++;
	}

	closeproc(processes);

	if (info->selected.current == NULL) {
		info->selected = procbst_cursor_init(&info->procs);
		procbst_cursor_last(&info->selected);
	}

	// delete processes that were not "found" in the last readproc cycle
	procbst_cursor_t cur = procbst_cursor_init(&info->procs);
	procbst_cursor_next(&cur);
	while (cur.current != NULL) {
		if (!(cur.current->value.flags & PROCINFO_FOUND)) {

			if (procbst_cursor_eq(cur, info->selected)) {
				procbst_cursor_prev(&info->selected);
			}

			procbst_dynamic_remove(&cur);
			num_procs--;
		} else {
			procbst_cursor_next(&cur);
		}
	}

	info->num_procs = num_procs;
	return info->num_procs;
}

procinfo_t proc_getinfo(proc_t proc, ptime_t period) {

	procinfo_t p;

	p.flags = 0;

#ifdef CMTOP_PROC_DRAW
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
	p.user 		= (char*) x_malloc((userlen + 1), sizeof(char));
	p.user[userlen] = '\0';
	x_strncpy(p.user, user_ptr, userlen);

	const char *cmd_ptr;

	if (proc.cmdline != NULL) {
		cmd_ptr = strip_pathinfo(proc.cmd);

		size_t cmd_len = strlen(cmd_ptr);
		p.cmd = x_malloc((cmd_len + 1), sizeof(char));
		x_strncpy(p.cmd, cmd_ptr, cmd_len);
		//free(*proc.cmdline);
	} else {
		p.cmd = null_cmd;
	}


	proc_updateinfo(&p, proc, period);

	return p;
}

void proc_freeinfo(procinfo_t* p_info) {
	x_free(p_info->user);

	if (p_info->cmd != null_cmd) {
		x_free(p_info->cmd);
	}
}

#include "common.h"
#include "xutil.h"
#include "error.h"
#include "procbst.h"
#include "proc.h"

#define PROCINFO_BUFSZ 8192
#define CPU_PERIOD_EPS 1E-6
#define I_EPS(val, eps) (((val) < (eps)) ? 0 : (val))
#define F_EPS(val, eps) (((val) < (eps)) ? 0.0F : (val))

const char* null_cmd = "\0";

static long jiffy = 1;

#define PROCINFO_FOUND 0x00001

/*
procinfo_t get_procinfo(pid_t pid) {
	// this should be sufficient for the stat output
	char buffer[PROCINFO_BUFSZ];

	FILE *fp;

	// print filename into buffer so we don't have to
	// allocate another one just for that...
	
	snprintf(buffer, PROCINFO_BUFSZ - 1, "/proc/%d/stat", pid);

	if ((fp = fopen(buffer, "r")) == NULL) {
		fatal(1,"could not open stat file for process %d", pid);
	}

	memset(buffer, '\0', PROCINFO_BUFSZ);

	size_t nread = fread(buffer, sizeof(char), PROCINFO_BUFSZ, fp);
	fclose(fp);

	// complete output of /proc/<pid>/stat is defined in:
	// https://github.com/torvalds/linux/blob/master/fs/proc/array.c
	// also see `man 5 proc` for explanations of output
	

	procinfo_t p;

	struct sysinfo s_info;
	sysinfo(&s_info);
	uint32_t clock_hertz = sysconf(_SC_CLK_TCK);

	char *token;
#define START() { token = strtok(buffer, " "); }
#define ADVANCE() { token = strtok(NULL, " "); }

	START();		// pid
	p.pid = atoi(token);
	ADVANCE();		// command
	p.command = token;
	ADVANCE();		// state
	p.state = *token;
	ADVANCE();		// parent pid
	ADVANCE();		// process group id
	ADVANCE();		// session id
	ADVANCE();		// tty nr
	ADVANCE();		// tty pgrp
	ADVANCE();		// task flags
	ADVANCE();		// # of minor faults
	ADVANCE();		// # of child min faults
	ADVANCE();		// # of maj faults
	ADVANCE();		// # of child maj faults

	clock_t total_time = 0;
	ADVANCE();		// user mode time
	total_time += atoi(token);
	ADVANCE();		// kernele mode time
	total_time += atoi(token);
	ADVANCE();		// child utime
	ADVANCE();		// child stime

	

	ADVANCE();		// priority
	p.priority = atoi(token);
	ADVANCE();		// nice value
	p.nice = atoi(token);
	ADVANCE();		// # of threads
	ADVANCE();		// 0
	ADVANCE();		// start_time
	long start_time = atol(token);
	ADVANCE();		// vsize

	long total_seconds = s_info.uptime - (start_time / clock_hertz);
	float cpu_usage = 100 * ((total_time / clock_hertz) / total_seconds);
	p.cpu_pct = cpu_usage;

	// mm fields
	ADVANCE();		// resident set size
	ADVANCE();		// rss limit
	ADVANCE();		// start code
	ADVANCE();		//  end code
	ADVANCE();		// start stack
	ADVANCE();		// current sp
	ADVANCE();		// current ip

	// obsolete signal info (4x)
	ADVANCE();		// 
	ADVANCE();		// 
	ADVANCE();		// 
	ADVANCE();		// 

	ADVANCE();		// wchan
	ADVANCE();		// 0
	ADVANCE();		// 0

	// task fields
	ADVANCE();		// exit signal
	ADVANCE();		// cpu # last executed on
	ADVANCE();		// rt priority
	ADVANCE();		// policy
	ADVANCE();		// aggergated block io delays
	ADVANCE();		// guest time
	ADVANCE();		// child guest time

	// more mm fields (if permitted and with mm)
	ADVANCE();		// start data
	ADVANCE();		// end data
	ADVANCE();		// start brk
	ADVANCE();		// arg start
	ADVANCE();		// arg end
	ADVANCE();		// env start
	ADVANCE();		//  env end

	// if permitted
	ADVANCE();		// task exit code
	return p;

}
*/

static inline ptime_t adjust_time(ptime_t time) {
	// this function probably won't do anything if the 
	// jiffy value is set to hundredths of a second 
	// (likely on modern linux kernels...)
	return time * 100 / jiffy;
}

procs_info_t procs_init() {

	procs_info_t pi;

	pi.num_procs = 0;
	pi.procs = procbst_init();
	pi.refresh_rate = 1000 * 100;
	memset(&pi.cpuinfo, 0, sizeof(sys_cpuinfo_t));

	jiffy = sysconf(_SC_CLK_TCK);
	printf("got jiffy %ld\n", jiffy);

	return pi;
}

int read_cpuinfo(sys_cpuinfo_t *info_ptr) {

#define CPUINFO_BUFSZ 256



	char buf[CPUINFO_BUFSZ];
	memset(buf, '\0', CPUINFO_BUFSZ);
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


void proc_updateinfo(procinfo_t* pi_ptr, proc_t proc, ptime_t period) {

	// update all the non-constant fields 
	// (everything except pid, user, cmd, stuff like that)...

	pi_ptr->priority 	= proc.priority;
	pi_ptr->nice 		= proc.nice;
	pi_ptr->virt_mem	= 0;
	pi_ptr->res_mem		= 0;
	pi_ptr->shr_mem		= 0;
	pi_ptr->state		= proc.state;
	pi_ptr->cpu_pct		= 0;
	pi_ptr->mem_pct		= 0;
	pi_ptr->start_time	= proc.start_time;

	TIMEDELTA_UPDATE(pi_ptr->cpuavg.stime, adjust_time(proc.stime));
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.utime, adjust_time(proc.utime));
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.cutime, adjust_time(proc.cutime));
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.cstime, adjust_time(proc.cstime));
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.ttime, adjust_time(proc.stime + proc.utime));

	// from htop:
	// float percent_cpu = (period < 1E-6) ? 0.0F : ((lp->utime + lp->stime - lasttimes) / period * 100.0);


	timedelta_t cputotal = pi_ptr->cpuavg.ttime;
	ptime_t cputotal_delta = cputotal.delta;

	if (period > CPU_PERIOD_EPS) {
		float percent_cpu = cputotal.delta * 100.0F / period;
		pi_ptr->cpu_pct = percent_cpu;
	}

	printf("proc %d (%s): ", pi_ptr->pid, pi_ptr->cmd);
	printf("period: %llu \tproc period: %llu\tcpu_pct: %f\n", period, cputotal.delta, pi_ptr->cpu_pct);

	return;
}

size_t procs_update(procs_info_t *info) {

	read_cpuinfo(&info->cpuinfo);

	PROCTAB* processes = openproc(
			PROC_FILLMEM | 
			PROC_FILLSTAT | 
			PROC_FILLSTATUS | 
			PROC_FILLUSR | 
			PROC_FILLCOM
		);

	while (1) {
		proc_t proc;
		memset(&proc, 0, sizeof(proc));

		if (
			readproc(processes, &proc) == NULL 
		) break;

		procinfo_t* proc_ptr = NULL;

		ptime_t period = info->cpuinfo.total.delta;

		if ((proc_ptr = procbst_find(&info->procs, PROC_PIDOF(proc))) != NULL) {
			proc_updateinfo(proc_ptr, proc, period);
		} else {
			procbst_insert(&info->procs, proc_getinfo(proc, period));
		}

	}

	closeproc(processes);

	return info->num_procs;
}

static char* strip_pathinfo(char* cmd_path) {
	char* current = cmd_path;

	size_t last_fslash = 0;

	for (size_t i = 0; *current != '\0'; i++, current++) {
		if (*current == '/') last_fslash = i;
	}

	return (char*) (cmd_path + last_fslash);
}

procinfo_t proc_getinfo(proc_t proc, ptime_t period) {

	procinfo_t p;

	p.flags = 0 & ~(PROCINFO_FOUND);

	// reference defn of proc_t:
	// https://github.com/thlorenz/procps/blob/master/deps/procps/proc/readproc.h
	
	p.pid 		= PROC_PIDOF(proc);

	const char *user_ptr = PROC_USEROF(proc);

	size_t userlen = strlen(user_ptr);
	p.user 		= (char*) malloc((userlen + 1) * sizeof(char));
	p.user[userlen] = '\0';
	x_strncpy(p.user, user_ptr, userlen);

	const char *cmd_ptr;

	if (proc.cmdline != NULL) {
		cmd_ptr = strip_pathinfo(proc.cmd);

		size_t cmd_len = strlen(cmd_ptr);
		p.cmd = malloc((cmd_len + 1) * sizeof(char));
		x_strncpy(p.cmd, cmd_ptr, cmd_len);
	} else {
		p.cmd = null_cmd;
	}

	proc_updateinfo(&p, proc, period);

	return p;
}


void proc_freeinfo(procinfo_t p_info) {
	free(p_info.user);

	if (p_info.cmd != null_cmd) {
		free(p_info.cmd);
	}
}

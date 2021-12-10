#include "common.h"
#include "error.h"
#include "procbst.h"
#include "proc.h"

#define PROCINFO_BUFSZ 8192

const char* null_cmd = "\0";

#define TIMEDELTA_UPDATE(TD, NEWTIME) {(TD).last = (TD).current; (TD).current = NEWTIME; (TD).delta = (TD).current - (TD).last;}

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

procs_info_t procs_init() {

	procs_info_t pi;

	pi.num_procs = 0;
	pi.procs = procbst_init();
	pi.cpuinfo = (sys_cpuinfo_t) {0,0};
	pi.refresh_rate = 1000 * 100;


	proc_collection_itf i;

	i.get = &procbst_find;
	i.add = &procbst_insert;
	i.remove = &procbst_remove;

	pi.procs_itf = i;

	return pi;
}

int read_cpuinfo(sys_cpuinfo_t *info_ptr) {

#define CPUINFO_BUFSZ 256

	FILE* statfile = fopen("/proc/stat", "r");

	char buf[CPUINFO_BUFSZ];
	memset(buf, '\0', CPUINFO_BUFSZ);
	fgets(buf, CPUINFO_BUFSZ - 1, statfile);

	// XXX: there is likely a better way to do this.

	char *valstart = buf + 3;

	while (*valstart == ' ') valstart++;

	unsigned long long user, nice, system, idle;
	sscanf(valstart, "%llu %llu %llu %llu", 
		&user,
		&nice,
		&system,
		&idle
	);

	TIMEDELTA_UPDATE(info_ptr->user, user);
	TIMEDELTA_UPDATE(info_ptr->nice, nice);
	TIMEDELTA_UPDATE(info_ptr->system, system);
	TIMEDELTA_UPDATE(info_ptr->idle, idle);

#undef CPUINFO_BUFSZ
}


void proc_updateinfo(procinfo_t* pi_ptr, proc_t proc) {

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

	TIMEDELTA_UPDATE(pi_ptr->cpuavg.stime, proc.stime);
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.utime, proc.utime);
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.ttime, proc.stime + proc.utime);

	return;
}

size_t procs_update(procs_info_t *info) {

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

		if ((proc_ptr = info->procs_itf.get(&info->procs, PROC_PIDOF(proc))) != NULL) {
			proc_updateinfo(proc_ptr, proc);
		} else {
			info->procs_itf.add(&info->procs, proc_getinfo(proc));
		}

	}

	closeproc(processes);

	return info->num_procs;
}

procinfo_t proc_getinfo(proc_t proc) {

	procinfo_t p;

	p.flags = 0 & ~(PROCINFO_FOUND);

	// reference defn of proc_t:
	// https://github.com/thlorenz/procps/blob/master/deps/procps/proc/readproc.h
	
	p.pid 		= PROC_PIDOF(proc);

	const char *user_ptr = PROC_USEROF(proc);

	size_t userlen = strlen(user_ptr);
	p.user 		= (char*) malloc((userlen + 1) * sizeof(char));
	p.user[userlen] = '\0';
	strncpy(p.user, user_ptr, userlen);

	const char *cmd_ptr;

	if (proc.cmdline != NULL) {
		cmd_ptr = *(proc.cmdline + 0);
		
		// parse out those pesky filesystem things
		// e.g. './hello.o' -> 'hello'
		char period_delim = '.';
		while (*cmd_ptr == '/' || *cmd_ptr == ' ' || *cmd_ptr == '-' || *cmd_ptr == period_delim) {
			if (*cmd_ptr == period_delim) period_delim = ' ';
			cmd_ptr++;
		}

		size_t cmd_len = strlen(cmd_ptr);
		p.cmd = malloc((cmd_len + 1) * sizeof(char));
		strncpy(p.cmd, cmd_ptr, cmd_len);
	} else {
		p.cmd = null_cmd;
	}

	proc_updateinfo(&p, proc);

	return p;
}


void proc_freeinfo(procinfo_t p_info) {
	free(p_info.user);

	if (p_info.cmd != null_cmd) {
		free(p_info.cmd);
	}
}

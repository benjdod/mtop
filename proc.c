#include "proc.h"
#include "common.h"
#include "error.h"

#define PROCINFO_BUFSZ 8192

const char* null_cmd = "\0";

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

size_t proc_getall(procinfo_t *proc_buffer, size_t n) {

	PROCTAB* processes = openproc(
			PROC_FILLMEM | 
			PROC_FILLSTAT | 
			PROC_FILLSTATUS | 
			PROC_FILLUSR | 
			PROC_FILLCOM);

	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));

	size_t i = 0;

	for ( ; i < n && readproc(processes, &proc_info) != NULL; i++) {
		proc_buffer[i] = proc_getinfo(proc_info);
	}

	closeproc(processes);

	return i;
}

procinfo_t proc_getinfo(proc_t proc) {
	procinfo_t p;

	// reference defn of proc_t:
	// https://github.com/thlorenz/procps/blob/master/deps/procps/proc/readproc.h
	//
	
	p.pid 		= proc.tid;

	const char *user_ptr = proc.ruser;

	size_t userlen = strlen(user_ptr);
	p.user 		= malloc((userlen + 1) * sizeof(char));
	strncpy(p.user, user_ptr, userlen);

	p.priority 	= proc.priority;
	p.nice 		= proc.nice;
	p.virt_mem	= 0;
	p.res_mem	= 0;
	p.shr_mem	= 0;
	p.state		= proc.state;
	p.cpu_pct	= 0;
	p.mem_pct	= 0;
	p.time		= proc.start_time;

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

	return p;
}

void proc_freeinfo(procinfo_t p_info) {
	free(p_info.user);

	if (p_info.cmd != null_cmd) {
		free(p_info.cmd);
	}
}

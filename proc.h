#ifndef _CMTOP_PROC_H
#define _CMTOP_PROC_H

#include "common.h"

typedef struct {
	unsigned long long 
		last,
		current,
		delta;
} timedelta_t;

typedef struct {
	timedelta_t 
		utime,
		stime,
		ttime;
} proc_cpuavg_t;

typedef struct {
	uint16_t flags;
	pid_t pid;
	char 
		*user, 
		*cmd;
	long priority;
	int32_t nice;
	uint32_t 
		virt_mem,
		res_mem,
		shr_mem,
		start_time;
	char state;
	float cpu_pct;
	float mem_pct;
	proc_cpuavg_t cpuavg;
} procinfo_t;

#define PROCINFO_FOUND 0x0001

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser

typedef struct {
	timedelta_t
		user,
		nice, 
		system,
		idle;
} sys_cpuinfo_t;

//#define TIMEDELTA_SHIFT(TD) {(TD).last = (TD).current;}
#define TIMEDELTA_UPDATE(TD, NEWTIME) {(TD).last = (TD).current; (TD).current = NEWTIME; (TD).delta = (TD).current - (TD).last;}

typedef struct {
	void (*init)();
	procinfo_t (*get)(pid_t pid);
	void (*remove)(pid_t pid);
} proc_collection_itf;

typedef struct {
	size_t num_procs;
	size_t proc_bufsize;
	procinfo_t *procs;
	sys_cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
} procs_info_t;


// procinfo_t get_procinfo(pid_t pid);
procinfo_t proc_getinfo(proc_t proc);
void proc_freeinfo(procinfo_t p_info);

#endif

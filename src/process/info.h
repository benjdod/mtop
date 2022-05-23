#ifndef MTOP_PROCESS_INFO_H
#define MTOP_PROCESS_INFO_H

#include "common.h"

/* This header contains definitions for types that deal with process and 
 * system information. */

// XXX: this is maybe too big for our needs.
typedef unsigned long long ptime_t;

/** Represents a change in time (delta = current - last).  */
typedef struct timedelta_t_ {
	ptime_t 
		last,
		current,
		delta;
} timedelta_t;


/** Update a time delta with a new time value. */
#define TIMEDELTA_UPDATE(TD, NEWTIME) { \
	(TD).last = (TD).current; \
	(TD).current = (NEWTIME); \
	(TD).delta = (TD).current - (TD).last;\
	}

/** Update a time delta if the new time is > 0 */
#define TIMEDELTA_COND_UPDATE(TD, NEWTIME) {if ((NEWTIME) > 0) TIMEDELTA_UPDATE((TD), (NEWTIME))}

// SYSTEM INFORMATION

/** Time deltas for CPU execution contexts. */
typedef struct cputimes_t_ {
	timedelta_t
		user,
		nice, 
		system,
		idle,
		iowait,
		irq,
		softirq,
		steal,
		guest,
		guest_nice,
		total;
} cputimes_t;

/** CPU and CPU core(s) information */
typedef struct cpuinfo_t_ {
	cputimes_t times;
	u16 num_cores;
	cputimes_t* coretimes;
} cpuinfo_t;

/** System memory information */
typedef struct meminfo_t_ {
	u64 total;
	u64 free;
	u64 available;
	u64 swap_total;
	u64 swap_free;
} meminfo_t;

/* System information */
typedef struct sysinfo_t_ {
	size_t num_procs;
	size_t running;
	cpuinfo_t cpu;
	meminfo_t mem;
} sysinfo_t;


// PROCESS INFORMATION

/* Process command line args */
typedef struct cmdline_args_t_ {
	int argc;
	char** argv;
} cmdline_args_t;

/* Process-specific CPU context time deltas. */
typedef struct proc_cpuavg_t_ {
	timedelta_t 
		utime,      // user time
		stime,      // system (kernel) time
		cutime,     // child user time
		cstime,     // child system time
		ttime;      // total
} proc_cpuavg_t;

/** Process-specific information. */
typedef struct procinfo_t_ {
	//uint16_t flags;
	pid_t pid;
	char 
		*user, 
		*cmd;
	long priority;
	i32 nice;
	u64 
		virt_mem,
		res_mem,
		shr_mem,
		start_time;
	char state;
	float 
		cpu_pct,
		mem_pct;
	proc_cpuavg_t cpuavg;
	cmdline_args_t args;
} procinfo_t;

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser

#endif
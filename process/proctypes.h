#ifndef _CMTOP_PROCTYPES_H
#define _CMTOP_PROCTYPES_H

#include "common.h"

typedef unsigned long long ptime_t;

typedef struct timedelta_t_ {
	ptime_t 
		last,
		current,
		delta;
} timedelta_t;

#ifdef CMTOP_PROC_DRAW
#define DRAWDATA_CACHE_LENGTH 8192

#define PROCINFO_ACTIVE	0x0002	// 0x001 is the found flag, defined in proc.c

// ..offset...###length#####...padding.....
// |---|---|---|---|---|---|---|---|---|---

typedef struct drawdata_t_ {
	int offset;
	size_t
		length,
		padding;
	char cache[DRAWDATA_CACHE_LENGTH];
} drawdata_t;
#endif

/** Updates a timedelta struct given a new time value. */
#define TIMEDELTA_UPDATE(TD, NEWTIME) { \
	(TD).last = (TD).current; \
	(TD).current = (NEWTIME); \
	(TD).delta = (TD).current - (TD).last;\
	}

#define TIMEDELTA_COND_UPDATE(TD, NEWTIME) {if ((NEWTIME) > 0) TIMEDELTA_UPDATE((TD), (NEWTIME))}

typedef struct proc_cpuavg_t_ {
	timedelta_t 
		utime,
		stime,
		cutime, 
		cstime,
		ttime;
} proc_cpuavg_t;

typedef struct procinfo_t_ {
	uint16_t flags;
#ifdef CMTOP_PROC_DRAW
	drawdata_t drawdata;
#endif
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
} procinfo_t;

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser


//#define TIMEDELTA_SHIFT(TD) {(TD).last = (TD).current;}



#endif
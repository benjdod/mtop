#ifndef _CMTOP_PROCTYPES_H
#define _CMTOP_PROCTYPES_H

#include "common.h"

typedef unsigned long long ptime_t;

typedef struct {
	ptime_t 
		last,
		current,
		delta;
} timedelta_t;

/** Updates a timedelta struct given a new time value. */
#define TIMEDELTA_UPDATE(TD, NEWTIME) { \
	(TD).last = (TD).current; \
	(TD).current = (NEWTIME); \
	(TD).delta = (TD).current - (TD).last;\
	}
		//(TD).current = ((NEWTIME) > (TD).current) ? (NEWTIME) : (TD).current; \
	//(TD).delta = ((NEWTIME) > (TD).current) ? (TD).current - (TD).last : 0;\

#define TIMEDELTA_COND_UPDATE(TD, NEWTIME) {if ((NEWTIME) > 0) TIMEDELTA_UPDATE((TD), (NEWTIME))}

typedef struct {
	timedelta_t 
		utime,
		stime,
		cutime, 
		cstime,
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
	float 
		cpu_pct,
		mem_pct;
	proc_cpuavg_t cpuavg;
} procinfo_t;

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser


//#define TIMEDELTA_SHIFT(TD) {(TD).last = (TD).current;}



#endif
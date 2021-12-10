#ifndef _CMTOP_PROCTYPES_H
#define _CMTOP_PROCTYPES_H

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

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser


//#define TIMEDELTA_SHIFT(TD) {(TD).last = (TD).current;}



#endif
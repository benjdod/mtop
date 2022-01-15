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

#ifndef MTOP_PROCTYPES_H
#define MTOP_PROCTYPES_H

#include "common.h"

typedef unsigned long long ptime_t;

typedef struct timedelta_t_ {
	ptime_t 
		last,
		current,
		delta;
} timedelta_t;

#ifdef MTOP_PROC_DRAW
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

typedef struct cpuinfo_t_ {
	cputimes_t total;
	u16 num_cores;
	cputimes_t* cores;
} cpuinfo_t;

typedef struct procinfo_t_ {
	uint16_t flags;
#ifdef MTOP_PROC_DRAW
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

typedef struct sysinfo_t_ {
	size_t num_procs;
	size_t running;
	cpuinfo_t cpu;
} sysinfo_t;

#define PROC_PIDOF(PROC) (PROC).tid
#define PROC_USEROF(PROC) (PROC).ruser


//#define TIMEDELTA_SHIFT(TD) {(TD).last = (TD).current;}



#endif

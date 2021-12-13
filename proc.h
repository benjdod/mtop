#ifndef _CMTOP_PROC_H
#define _CMTOP_PROC_H

#include "common.h"
#include "proctypes.h"
#include "procbst.h"

typedef struct {
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
} sys_cpuinfo_t;

typedef struct {
	size_t num_procs;
	procbst_t procs;
	sys_cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
} procs_info_t;

procs_info_t procs_init();
size_t procs_update(procs_info_t*);
int read_cpuinfo(sys_cpuinfo_t*);
procinfo_t proc_getinfo(proc_t proc, ptime_t period);
void proc_freeinfo(procinfo_t p_info);

#endif

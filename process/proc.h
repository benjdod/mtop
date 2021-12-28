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
} cputimes_t;

typedef struct cpuinfo_t_ {
	cputimes_t total;
	u16 num_cores;
	cputimes_t* cores;
} cpuinfo_t;

typedef struct {
	size_t num_procs;
	procbst_t procs;
	cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
	procbst_cursor_t selected;
} procs_info_t;

const char* proc_state_tostring(char state);
size_t proc_state_getstring(char state, char* buf);
procs_info_t procs_init();
void procs_destroy(procs_info_t* procs);
size_t procs_update(procs_info_t*);
procinfo_t proc_getinfo(proc_t proc, ptime_t period);
void proc_freeinfo(procinfo_t* p_info);
void procs_selectnext(procs_info_t*);
void procs_selectprev(procs_info_t*);
procinfo_t* procs_selected(procs_info_t*);

#endif

#ifndef MTOP_PROC_H
#define MTOP_PROC_H

#include "common.h"
#include "proctypes.h"
#include "proclist.h"

#define PROCS_SELECT_FIRST 	0x1
#define PROCS_SELECT_LAST 	0x2
#define PROCS_SELECT_NEXT 	0x3
#define PROCS_SELECT_PREV   0x4

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

typedef struct procs_info_t_ {
	size_t num_procs;
	proclist_t procs;
	cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
	proclist_cur_t selected;
#ifdef MTOP_PROC_DRAW
	size_t selected_index;
	size_t draw_offset;
#endif
} procs_info_t;

const char* proc_state_tostring(char state);
size_t proc_state_getstring(char state, char* buf);
procs_info_t procs_init();
size_t procs_select(procs_info_t* info, u8 select);
void procs_destroy(procs_info_t* procs);
size_t procs_update(procs_info_t*);
procinfo_t proc_getinfo(proc_t proc, ptime_t period);
void proc_freeinfo(procinfo_t* p_info);
procinfo_t* procs_selected(procs_info_t*);

#endif

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
		idle;
} sys_cpuinfo_t;

typedef procbst_t proc_collection_t;

typedef struct {
	proc_collection_t 		(*init)		();
	void 					(*add)		(proc_collection_t*, procinfo_t);
	procinfo_t*				(*get)		(proc_collection_t*, pid_t);
	void 					(*remove)	(proc_collection_t*, pid_t);
	void 					(*inorder)	(proc_collection_t*, void(*onvalue)(procinfo_t));
} proc_collection_itf;

typedef struct {
	size_t num_procs;
	procbst_t procs;
	sys_cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
	proc_collection_itf procs_itf;
} procs_info_t;

procs_info_t procs_init();
size_t procs_update(procs_info_t*);
procinfo_t proc_getinfo(proc_t proc);
void proc_freeinfo(procinfo_t p_info);

#endif

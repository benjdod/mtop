#ifndef _CMTOP_PROC_H
#define _CMTOP_PROC_H

#include "common.h"

typedef struct {
	pid_t pid;
	char *user;
	long priority;
	int32_t nice;
	uint32_t virt_mem;
	uint32_t res_mem;
	uint32_t shr_mem;
	char state;
	float cpu_pct;
	float mem_pct;
	uint32_t time;
	char *cmd;
} procinfo_t;

// procinfo_t get_procinfo(pid_t pid);
size_t proc_getall(procinfo_t *proc_buffer, size_t n);
procinfo_t proc_getinfo(proc_t proc);
void proc_freeinfo(procinfo_t p_info);

#endif

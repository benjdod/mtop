#include "proc.h"
#include "common.h"
#include "error.h"

#define PROCINFO_BUFSZ 8192

const char* null_cmd = "\0";

/*
procinfo_t get_procinfo(pid_t pid) {
	// this should be sufficient for the stat output
	char buffer[PROCINFO_BUFSZ];

	FILE *fp;

	// print filename into buffer so we don't have to
	// allocate another one just for that...
	
	snprintf(buffer, PROCINFO_BUFSZ - 1, "/proc/%d/stat", pid);

	if ((fp = fopen(buffer, "r")) == NULL) {
		fatal(1,"could not open stat file for process %d", pid);
	}

	memset(buffer, '\0', PROCINFO_BUFSZ);

	size_t nread = fread(buffer, sizeof(char), PROCINFO_BUFSZ, fp);
	fclose(fp);

	// complete output of /proc/<pid>/stat is defined in:
	// https://github.com/torvalds/linux/blob/master/fs/proc/array.c
	// also see `man 5 proc` for explanations of output
	

	procinfo_t p;

	struct sysinfo s_info;
	sysinfo(&s_info);
	uint32_t clock_hertz = sysconf(_SC_CLK_TCK);

	char *token;
#define START() { token = strtok(buffer, " "); }
#define ADVANCE() { token = strtok(NULL, " "); }

	START();		// pid
	p.pid = atoi(token);
	ADVANCE();		// command
	p.command = token;
	ADVANCE();		// state
	p.state = *token;
	ADVANCE();		// parent pid
	ADVANCE();		// process group id
	ADVANCE();		// session id
	ADVANCE();		// tty nr
	ADVANCE();		// tty pgrp
	ADVANCE();		// task flags
	ADVANCE();		// # of minor faults
	ADVANCE();		// # of child min faults
	ADVANCE();		// # of maj faults
	ADVANCE();		// # of child maj faults

	clock_t total_time = 0;
	ADVANCE();		// user mode time
	total_time += atoi(token);
	ADVANCE();		// kernele mode time
	total_time += atoi(token);
	ADVANCE();		// child utime
	ADVANCE();		// child stime

	

	ADVANCE();		// priority
	p.priority = atoi(token);
	ADVANCE();		// nice value
	p.nice = atoi(token);
	ADVANCE();		// # of threads
	ADVANCE();		// 0
	ADVANCE();		// start_time
	long start_time = atol(token);
	ADVANCE();		// vsize

	long total_seconds = s_info.uptime - (start_time / clock_hertz);
	float cpu_usage = 100 * ((total_time / clock_hertz) / total_seconds);
	p.cpu_pct = cpu_usage;

	// mm fields
	ADVANCE();		// resident set size
	ADVANCE();		// rss limit
	ADVANCE();		// start code
	ADVANCE();		//  end code
	ADVANCE();		// start stack
	ADVANCE();		// current sp
	ADVANCE();		// current ip

	// obsolete signal info (4x)
	ADVANCE();		// 
	ADVANCE();		// 
	ADVANCE();		// 
	ADVANCE();		// 

	ADVANCE();		// wchan
	ADVANCE();		// 0
	ADVANCE();		// 0

	// task fields
	ADVANCE();		// exit signal
	ADVANCE();		// cpu # last executed on
	ADVANCE();		// rt priority
	ADVANCE();		// policy
	ADVANCE();		// aggergated block io delays
	ADVANCE();		// guest time
	ADVANCE();		// child guest time

	// more mm fields (if permitted and with mm)
	ADVANCE();		// start data
	ADVANCE();		// end data
	ADVANCE();		// start brk
	ADVANCE();		// arg start
	ADVANCE();		// arg end
	ADVANCE();		// env start
	ADVANCE();		//  env end

	// if permitted
	ADVANCE();		// task exit code
	return p;

}
*/

/*
 * We need a way to insert and remove processes from a store with order guaranteed. 
 * This binary search tree will give us what we need for now.
 */

typedef struct {
	pitree_t *left, *right, *parent;
	procinfo_t value;
} pitree_t;

typedef struct {
	pitree_t 
		*tree,
		*current;
} pitree_cursor_t;

#define PITREE_COMPARE(A,B) ((int) (B).pid - (A).pid)

static pitree_t* pitree_node(procinfo_t info, pitree_t* left, pitree_t* right, pitree_t* parent) {
	pitree_t *pt = (pitree_t*) malloc(sizeof(pitree_t));
	pt->value = info;
	pt->left = left;
	pt->right = right;
	pt->parent = parent;
	return pt;
}

static pitree_t pitree_free(pitree_t* tree) {
	free(tree);
}

static void pitree_insert(pitree_t* tree, procinfo_t info) {
	pitree_t 
		*current = tree,
		**next = NULL;

	*next = tree;

	while (*next != NULL) {

		current = *next;

		if (PITREE_COMPARE(info, current->value) > 0) {
			next = &current->right;
		} else {
			next = &current->left;
		}
	}

	*next = pitree_node(info, NULL, NULL, current);
}

static procinfo_t* pitree_has(pitree_t* tree, procinfo_t info) {

	pitree_t* current = tree;

	while (current != NULL) {
		int compare = PITREE_COMPARE(info, current->value);
		if (compare == 0) {
			return &current->value;
		} else {
			current = (compare > 0)
				? current->left
				: current->right;
		}
	}
	return NULL;
}

static pitree_t* pitree_remove(pitree_t* tree, procinfo_t info) {

	pitree_t *current = tree, *parent = NULL, **parent_ptr = NULL;

	while (current != NULL) {
		int compare = PITREE_COMPARE(info, current->value);
		if (compare == 0) {
			break;
		} else {
			parent = current;
			parent_ptr = (compare > 0)
				? &current->left
				: &current->right;
			current = *parent_ptr;
		}
	}

	if (current == NULL) return tree;

	if (current->left != NULL && current->right != NULL) {	// 2 subtrees
		pitree_t *succ = NULL;

		// get successor
		succ = current->right;
		while (succ->left != NULL) succ = succ->left; 

		current->value = succ->value;
		pitree_remove(current->right, succ->value);

	} else if (current->left != NULL || current->right != NULL ) {		// 1 subtree
		pitree_t* target = (current->left != NULL) ? current->left : current->right;
		*parent_ptr = target;
		target->parent = parent;
	} else {	// no subtress
		*parent_ptr = NULL;
	}

	pitree_free(current);

	return tree;
}

static pitree_cursor_t pitree_cursor_init(pitree_t *tree) {
	pitree_cursor_t c;
	c.tree = tree;
	return c;
}

static const procinfo_t* pitree_cursor_first(pitree_cursor_t cursor) {
	if (cursor.current == NULL) cursor.current = cursor.tree;
	while (cursor.current->parent != NULL) cursor.current = cursor.current->parent;
	while (cursor.current->left != NULL) cursor.current = cursor.current->left;
	return &cursor.current->value;
}

static const procinfo_t* pitree_cursor_last(pitree_cursor_t cursor) {
	if (cursor.current == NULL) cursor.current = cursor.tree;
	while (cursor.current->parent != NULL) cursor.current = cursor.current->parent;
	while(cursor.current->right != NULL) cursor.current = cursor.current->right;
	return &cursor.current->value;
}

static const procinfo_t* pitree_cursor_next(pitree_cursor_t cursor) {
	if (cursor.current == NULL) return pitree_cursor_first(cursor);
	else {
		if (cursor.current->right != NULL) {
			cursor.current = cursor.current->right;
			while (cursor.current->left != NULL) cursor.current = cursor.current->left;
		} else {
			if (cursor.current->parent->right == cursor.current) {
				return NULL;
			}
			cursor.current = cursor.current->parent;
		}

		return &cursor.current->value;
	}
}

procs_info_t procs_init() {

	procs_info_t pi;

	pi.proc_bufsize = 512;
	pi.procs = (procinfo_t*) malloc(pi.proc_bufsize * sizeof(procinfo_t));
	pi.num_procs = 0;
	pi.refresh_rate = 1000 * 100;	// in usec
	pi.cpuinfo = (sys_cpuinfo_t) {0,0};

	return pi;
}

int read_cpuinfo(sys_cpuinfo_t *info_ptr) {

#define CPUINFO_BUFSZ 256

	FILE* statfile = fopen("/proc/stat", "r");

	char buf[CPUINFO_BUFSZ];
	memset(buf, '\0', CPUINFO_BUFSZ);
	fgets(buf, CPUINFO_BUFSZ - 1, statfile);

	// XXX: there is likely a better way to do this.

	char *valstart = buf + 3;

	while (*valstart == ' ') valstart++;

	unsigned long long user, nice, system, idle;
	sscanf(valstart, "%llu %llu %llu %llu", 
		&user,
		&nice,
		&system,
		&idle
	);

	TIMEDELTA_UPDATE(info_ptr->user, user);
	TIMEDELTA_UPDATE(info_ptr->nice, nice);
	TIMEDELTA_UPDATE(info_ptr->system, system);
	TIMEDELTA_UPDATE(info_ptr->idle, idle);

#undef CPUINFO_BUFSZ
}

size_t procs_update(procs_info_t *info) {

	PROCTAB* processes = openproc(
			PROC_FILLMEM | 
			PROC_FILLSTAT | 
			PROC_FILLSTATUS | 
			PROC_FILLUSR | 
			PROC_FILLCOM
		);


/*
	for (size_t i = 0; i < info->num_procs; i++) {
		proc_freeinfo(info->procs[i]);
	}
	info->num_procs = 0;
	memset(info->procs, '\0', sizeof(info->proc_bufsize * sizeof(procinfo_t)));
	*/

	while (1) {
		proc_t proc;
		memset(&proc, 0, sizeof(proc));

		if (
			readproc(processes, &proc) == NULL || 
			info->num_procs == info->proc_bufsize
		) break;

		/*
		TODO: filter by pid so that we don't have to constantly
		allocate and reallocate space for the processes.
		*/

		pid_t current_pid = PROC_PIDOF(proc);
		procinfo_t* proc_ptr = NULL;

		// filter for procs that are alredy present.
		// if so, just update them instead of reading again
		for (size_t i = 0; i < info->num_procs; i++) {
			if (info->procs[i].pid == current_pid) {
				proc_ptr = info->procs + i;
			}
		}

		if (proc_ptr != NULL) {
			proc_updateinfo(proc_ptr, proc);
		} else {
			info->procs[info->num_procs++] = proc_getinfo(proc);
		}
	}


	closeproc(processes);

	return info->num_procs;
}

procinfo_t proc_getinfo(proc_t proc) {

	procinfo_t p;

	p.flags = 0 & ~(PROCINFO_FOUND);

	// reference defn of proc_t:
	// https://github.com/thlorenz/procps/blob/master/deps/procps/proc/readproc.h
	
	p.pid 		= PROC_PIDOF(proc);

	const char *user_ptr = PROC_USEROF(proc);

	size_t userlen = strlen(user_ptr);
	p.user 		= (char*) malloc((userlen + 1) * sizeof(char));
	p.user[userlen] = '\0';
	strncpy(p.user, user_ptr, userlen);

	const char *cmd_ptr;

	if (proc.cmdline != NULL) {
		cmd_ptr = *(proc.cmdline + 0);
		
		// parse out those pesky filesystem things
		// e.g. './hello.o' -> 'hello'
		char period_delim = '.';
		while (*cmd_ptr == '/' || *cmd_ptr == ' ' || *cmd_ptr == '-' || *cmd_ptr == period_delim) {
			if (*cmd_ptr == period_delim) period_delim = ' ';
			cmd_ptr++;
		}

		size_t cmd_len = strlen(cmd_ptr);
		p.cmd = malloc((cmd_len + 1) * sizeof(char));
		strncpy(p.cmd, cmd_ptr, cmd_len);
	} else {
		p.cmd = null_cmd;
	}

	proc_updateinfo(&p, proc);

	return p;
}

void proc_updateinfo(procinfo_t* pi_ptr, proc_t proc) {

	// update all the non-constant fields 
	// (everything except pid, user, cmd, stuff like that)...

	pi_ptr->priority 	= proc.priority;
	pi_ptr->nice 		= proc.nice;
	pi_ptr->virt_mem	= 0;
	pi_ptr->res_mem		= 0;
	pi_ptr->shr_mem		= 0;
	pi_ptr->state		= proc.state;
	pi_ptr->cpu_pct		= 0;
	pi_ptr->mem_pct		= 0;
	pi_ptr->start_time	= proc.start_time;

	TIMEDELTA_UPDATE(pi_ptr->cpuavg.stime, proc.stime);
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.utime, proc.utime);
	TIMEDELTA_UPDATE(pi_ptr->cpuavg.ttime, proc.stime + proc.utime);

	return;
}

void proc_freeinfo(procinfo_t p_info) {
	free(p_info.user);

	if (p_info.cmd != null_cmd) {
		free(p_info.cmd);
	}
}

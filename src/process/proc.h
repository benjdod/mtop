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

#ifndef MTOP_PROCESS_PROC_H
#define MTOP_PROCESS_PROC_H

#include "common.h"
#include "info.h"

// select codes
#define PROCS_SELECT_FIRST 	0x1
#define PROCS_SELECT_LAST 	0x2
#define PROCS_SELECT_NEXT 	0x3
#define PROCS_SELECT_PREV   0x4

// window bitflags for the open_windows field
#define PROCS_WINDOW_NONE 0x0
#define PROCS_WINDOW_SYSINFO 0x1
#define PROCS_WINDOW_PROCINFO 0x2

#define DRAWDATA_CACHE_LENGTH 8192

/** Process-specific hashdata for interval generation */
typedef struct rand_hashdata_t_ {
	size_t base;
	size_t salt;
} rand_hashdata_t;

/** process-specific masking information (to make those sweet matrix lines!) */
typedef struct rand_drawctx_t_ {
	// hashdata for interval generation
	rand_hashdata_t hashdata;
	// the value (length) of the current interval
	int rand;
	// index of the current interval
	size_t index;
	// offset into the current interval
	size_t offset;
	// whether or not the current interval is visible
	int visible;
} rand_drawctx_t;

typedef struct ctx_cache_t_ {
	rand_drawctx_t ctx;
	size_t screen_offset;
} ctx_cache_t;

typedef struct drawdata_t_ {
	size_t offset;
	size_t length;
	rand_drawctx_t ctx;
	ctx_cache_t ctx_cache;
	size_t last_screen_offset;
	char cache[DRAWDATA_CACHE_LENGTH];
} drawdata_t;

typedef struct procnode_t_ procnode_t;
typedef struct proclist_t_ proclist_t;
typedef struct proclist_cur_t_ proclist_cur_t;

struct procnode_t_ {
    procinfo_t value;
	drawdata_t dd;
	u8 flags;
    procnode_t 
        *next,
        *prev;
};

struct proclist_cur_t_ {
    proclist_t* list;
    procnode_t* current;
	size_t index;
    u8 pos;
};

typedef enum {
	CUR_SHIFT_LEFT = 0,
	CUR_SHIFT_RIGHT
} proclist_cur_shift ;

struct proclist_t_ {
    procnode_t
        *head, 		// list head 
        *tail, 		// list tail
		*search; 	// the most recently found node (pl_findnode)
    size_t size;
};

#define PL_CUR_FOREACH(cursor) for (pl_cur_next(cursor) ; (cursor)->current != NULL; pl_cur_next(cursor))

typedef struct procs_info_t_ {
	sysinfo_t sys;
	proclist_t procs;
	uint32_t refresh_rate;
	proclist_cur_t selected;
	size_t selected_index;
	size_t col_offset;
	size_t row_offset;
	u16 open_windows;

	// display_size = real_size * step
	//
	// +-----------+
	// |  |  |  |       4 * 3 = 12
	// | | | | | | 		6 * 2 = 12
	// |||||||||||| 	12 * 1 = 12
	
	size_t display_size; 	
	size_t real_size;
	size_t step;
} procs_info_t;

void procs_foreachnode(procs_info_t* info, void (*on_value)(procnode_t*));
void procs_foreach(procs_info_t* info, void (*on_value)(procinfo_t*));

proclist_cur_t pl_cur_init(proclist_t* list);
proclist_cur_t pl_cur_clone(proclist_cur_t* cursor);
procnode_t* pl_cur_at(proclist_cur_t* cursor);
procnode_t* pl_cur_next(proclist_cur_t* cursor);
u8 pl_cur_eq(proclist_cur_t* a, proclist_cur_t* b);

const char* proc_state_tostring(char state);
size_t proc_state_getstring(char state, char* buf);
procs_info_t procs_init();
void procs_set_drawopts(procs_info_t* info, size_t step, size_t rsize, size_t csize);
size_t procs_select(procs_info_t* info, u8 select);
void procs_destroy(procs_info_t* procs);
size_t procs_update(procs_info_t*);
procinfo_t proc_getinfo(proc_t proc, ptime_t period);
void proc_freeinfo(procinfo_t* p_info);
procinfo_t* procs_selected(procs_info_t*);

#endif

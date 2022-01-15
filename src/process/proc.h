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

#ifndef MTOP_PROC_H
#define MTOP_PROC_H

#include "common.h"
#include "proctypes.h"
#include "proclist.h"

#define PROCS_SELECT_FIRST 	0x1
#define PROCS_SELECT_LAST 	0x2
#define PROCS_SELECT_NEXT 	0x3
#define PROCS_SELECT_PREV   0x4

typedef struct procs_info_t_ {
	//size_t num_procs;
	sysinfo_t sys;
	proclist_t procs;
	//cpuinfo_t cpuinfo;
	uint32_t refresh_rate;
	proclist_cur_t selected;
#ifdef MTOP_PROC_DRAW
	size_t selected_index;
	size_t draw_offset;

	// display_size = query_size * step
	//
	// +-----------+
	// |  |  |  |       4 * 3 = 12
	// | | | | | | 		6 * 2 = 12
	// |||||||||||| 	12 * 1 = 12
	
	size_t display_size; 	
	size_t real_size;
	size_t step;
#endif
} procs_info_t;

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

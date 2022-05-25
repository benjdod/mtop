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

#include "common.h"

#ifndef MTOP_PROCESS_DRAW_H
#define MTOP_PROCESS_DRAW_H

#include "proc.h"
#include "display/dcolor.h"

typedef struct cchar_t_ {
	dcolor_t color;
	char c;
} cchar_t;

char randchar();

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, u8 section);
size_t pd_drawcpuinfo(cpuinfo_t cpuinfo, char* buf, size_t n, u8 section);
size_t pd_drawto(procinfo_t* p, char* buf, size_t n);
rand_drawctx_t pd_init_drawctx(pid_t pid);

/* advance drawctx visual offset by one, rolling to the next interval as appropriate. */ 
void pd_advance_drawctx(rand_drawctx_t* ctx);
/* advance drawctx visual offset to next interval. */
void pd_advance_drawctx_interval(rand_drawctx_t* ctx);
/* see pd_advance_drawctx() */
void pd_retract_drawctx(rand_drawctx_t* ctx);
/* see pd_advance_drawctx_interval() */
void pd_retract_drawctx_interval(rand_drawctx_t* ctx);
/* randomize visual values in drawctx */
void pd_random_drawctx(rand_drawctx_t* ctx);

/* returns whether or not the point at screen offset is unmasked or masked */
int randd_visible(rand_drawctx_t ctx, size_t screen_offset);
/* return the character at the specified screen offset */
char pd_charat(procnode_t* p, size_t screen_offset);
/* return the color and character at the specified screen offset */
cchar_t pd_ccharat(procnode_t* p, size_t screen_offset);
/* return an interval value for the hashdata and index combination */
int pd_get_interval(rand_hashdata_t hashdata, size_t index);
/* update a procnode's internal draw cache */
void pd_updatecache(procnode_t* p);

#endif
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

#include "procdraw.h"
#include "proc.h"

size_t pd_drawto(procinfo_t* p, char* buf, size_t n) {
//    printf("pd drawing to %p\n", buf);

    char* cmd_str = (*(p->cmd) == '\0')
        ? "-"
        : p->cmd;

    proc_cpuavg_t cpuavg = p->cpuavg;
    int written = snprintf(
		buf,
        n,
		"%s (%d) %c %llu %llu %llu %llu %.2f", 
		cmd_str, 
		p->pid,
		p->state,
		cpuavg.utime.current,
		cpuavg.stime.current,
		cpuavg.cutime.current,
		cpuavg.cstime.current,
        p->cpu_pct
	);

    return (written >= n) ? n : ((size_t) written);
}

static void truncate_buffer(char* buf, size_t w, size_t n) {

    if (w >= n) {
        size_t i = n-1;

        while (i && buf[i] != ' ') {
            buf[i] = ' ';
            i -= 1;
        }
    } else {
        while (w < n) {
            buf[w++] = ' ';
        }
    }
    
    return;
}

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, u8 section) {

    size_t w = 0;

#define CLIP_BUF() 

    proc_cpuavg_t ct = p->cpuavg;

    switch (section) {
        case 0:
            w = snprintf(buf, n, "%s - %s (%d) %c (%s)", p->user, p->cmd, p->pid, p->state, proc_state_tostring(p->state));
            break;
        case 1:
            w = snprintf(buf, n, "%llu, %llu, %llu, %llu - clock ticks of execution (user, sys, cu, cs)", ct.utime.current, ct.stime.current, ct.cutime.current, ct.cstime.current);
            break;
        case 2:
            w = snprintf(buf, n, "%lu %lu %lu %.2f - memory and cpu (res, shared, vsz, cpu %%)", p->res_mem, p->shr_mem, p->virt_mem, p->cpu_pct);
            //w = snprintf(buf, n, "hello");
            break;
        default:
            return 0;
    }

    truncate_buffer(buf, w, n);
    return n;

}

size_t pd_drawcpuinfo(cpuinfo_t cpuinfo, char* buf, size_t n, u8 section) {
    size_t w = 0;

    switch (section) {
        case 0:
            w = snprintf(buf, n, "%llu - idle cpu!", cpuinfo.times.idle.current);
            break;
        default: 
            return 0;
    }

    /* if (w >= n) {
        //size_t i = w-1; 
        w -= 1;
        while(buf[w] != ' ' && w >= n) buf[w--] = ' ';
    } */

    truncate_buffer(buf, w, n);
    return n;
}

void pd_updatecache(procinfo_t* p) {
   // printf("updating cache at %p\n", p->drawdata.cache);
    p->drawdata.length = pd_drawto(p, p->drawdata.cache, DRAWDATA_CACHE_LENGTH);
}

inline char pd_charat(procinfo_t* p, size_t offset) {

    // XXX: overflows here? 
    // should we modulo after subtracting process offset?
    long idx = ((offset - p->drawdata.offset) % (p->drawdata.length + p->drawdata.padding));
    if (idx < 0) return ' ';

    char out = p->drawdata.cache[idx];

    return (out != '\0') ? out : ' ';
//	return '#';
}

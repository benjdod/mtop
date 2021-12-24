#include "procdraw.h"

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

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, unsigned short section) {

    size_t w = 0;

#define CLIP_BUF() 

    proc_cpuavg_t ct = p->cpuavg;

    switch (section) {
        case 0:
            w = snprintf(buf, n, "%s - %s (%d) %c", p->user, p->cmd, p->pid, p->state);
            break;
        case 1:
            w = snprintf(buf, n, "%llu, %llu, %llu, %llu - clock ticks of execution (user, sys, cu, cs)", ct.utime.current, ct.stime.current, ct.cutime.current, ct.cstime.current);
            break;
        case 2:
            w = snprintf(buf, n, "%u %u %.2f - memory and cpu (res, shared, cpu %%)", p->res_mem, p->shr_mem, p->cpu_pct);
            //w = snprintf(buf, n, "hello");
            break;
        default:
            return 0;
    }

    if (w >= n) {
        //size_t i = w-1; 
        w -= 1;
        while(buf[w] != ' ' && w >= n) buf[w--] = ' ';
    }

    return w;

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
}
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

void pd_updatecache(procinfo_t* p) {
   // printf("updating cache at %p\n", p->drawdata.cache);
    p->drawdata.length = pd_drawto(p, p->drawdata.cache, DRAWDATA_CACHE_LENGTH);
}

char pd_charat(procinfo_t* p, size_t offset) {

    // XXX: overflows here? 
    // should we modulo after subtracting process offset?
    long idx = ((offset - p->drawdata.offset) % (p->drawdata.length + p->drawdata.padding));
    if (idx < 0) return ' ';

    char out = p->drawdata.cache[idx];

    return (out != '\0') ? out : ' ';
}
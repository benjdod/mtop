#include "common.h"

#if !defined(MTOP_PROCDRAW_H) && defined(MTOP_PROC_DRAW)
#define MTOP_PROCDRAW_H

#include "proctypes.h"
#include "proc.h"

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, u8 section);
size_t pd_drawcpuinfo(cpuinfo_t cpuinfo, char* buf, size_t n, u8 section);
size_t pd_drawto(procinfo_t* p, char* buf, size_t n);
char pd_charat(procinfo_t* p, size_t offset);
void pd_updatecache(procinfo_t* p);

#endif
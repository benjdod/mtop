#include "common.h"

#if !defined(_CMTOP_PROCDRAW_H) && defined(CMTOP_PROC_DRAW)
#define _CMTOP_PROCDRAW_H

#include "proctypes.h"

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, unsigned short section);
size_t pd_drawto(procinfo_t* p, char* buf, size_t n);
char pd_charat(procinfo_t* p, size_t offset);
void pd_updatecache(procinfo_t* p);

#endif
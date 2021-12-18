#ifndef _COMMON_H

// we include os level dependencies here, and we can also shim missing functionality 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>	// struct sysinfo, sysinfo()
#include <string.h>		// strtok
#include <proc/readproc.h>	// readproc, openproc, closeproc
#include <time.h>

#define _COMMON_H

/* FLAGS */

#define CMTOP_PROC_DRAW
//#define CMTOP_DRAW_COLOR

#endif

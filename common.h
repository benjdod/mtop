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
#define CMTOP_DRAW_COLOR

// XXX: should we make these ourselves?

typedef     uint8_t     u8;
typedef     uint16_t    u16;
typedef     uint32_t    u32;
typedef     uint64_t    u64;
typedef     int8_t      i8;
typedef     int16_t     i16;
typedef     int32_t     i32;
typedef     int64_t     i64;


#endif

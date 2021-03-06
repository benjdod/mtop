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

#ifndef MTOP_COMMON_H
#define MTOP_COMMON_H

// we include os level dependencies here, and we can also shim missing functionality 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>	// struct sysinfo, sysinfo()
#include <proc/readproc.h>	// readproc, openproc, closeproc
#include <pthread.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// compile flags

// #define MTOP_MULTITHREAD 

// integer typedefs for sanity :)

typedef     uint8_t     u8;
typedef     uint16_t    u16;
typedef     uint32_t    u32;
typedef     uint64_t    u64; 
typedef     int8_t      i8;
typedef     int16_t     i16;
typedef     int32_t     i32;
typedef     int64_t     i64;

#endif

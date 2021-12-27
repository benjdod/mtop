#ifndef CMTOP_OPT_H
#define CMTOP_OPT_H

#include "common.h"



typedef struct cmtop_opt_t_ {
#define OPT_DRAWCOLOR_NONE     0x0
#define OPT_DRAWCOLOR_ANSI     0x1
#define OPT_DRAWCOLOR_8BIT     0x2
#define OPT_DRAWCOLOR_24BIT    0x3
    u8 colormode;


} cmtop_opt_t;

// global options structs
cmtop_opt_t opt;

void opt_init();

#endif
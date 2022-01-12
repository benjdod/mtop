#ifndef MTOP_OPT_H
#define MTOP_OPT_H

#include "common.h"

#define OPT_YES 0x1
#define OPT_NO  0x0

typedef struct mtop_opt_t_ {
#define OPT_DRAWCOLOR_NONE     0x0
#define OPT_DRAWCOLOR_ANSI     0x1
#define OPT_DRAWCOLOR_8BIT     0x2
#define OPT_DRAWCOLOR_24BIT    0x3
    u8 colormode;
    u8 draw_static;
    u32 refresh_rate;
} mtop_opt_t;

// global options structs
extern mtop_opt_t opt;

#define SET_OPT(FIELD, VALUE) {opt.FIELD = VALUE;}

void opt_default();
void opt_print();

#endif

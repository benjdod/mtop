#include "opt.h"

extern cmtop_opt_t opt;

void opt_init() {
    opt.colormode = OPT_DRAWCOLOR_24BIT;
    opt.draw_static = OPT_NO;
}
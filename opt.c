#include "opt.h"

mtop_opt_t opt;	// global options struct

void opt_init() {
    opt.colormode = OPT_DRAWCOLOR_24BIT;
    opt.draw_static = OPT_NO;
}

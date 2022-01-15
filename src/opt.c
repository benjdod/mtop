#include "opt.h"

mtop_opt_t opt;	// global options struct

void opt_default() {
    opt.colormode       = OPT_DRAWCOLOR_24BIT;
    opt.draw_static     = OPT_NO;
    opt.refresh_rate    = 100;
}

#define YESNO(B) (B) ? "yes" : "no"

#define COLORMODE_TOSTR(MODE) \
    (MODE == OPT_DRAWCOLOR_24BIT) \
        ? "24 bit" \
        : (MODE == OPT_DRAWCOLOR_8BIT) \
            ? "8 bit" \
            : (MODE = OPT_DRAWCOLOR_ANSI) \
                ? "ansi" \
                : "none" 

void opt_print() {
    printf("color mode:\t%s\n", COLORMODE_TOSTR(opt.colormode));
    printf("draw static:\t%s\n", YESNO(opt.draw_static));
    printf("refresh rate:\t%ums\n", opt.refresh_rate);
}

#include "opt.h"

mtop_opt_t opt;	// global options struct

void opt_init() {
	SET_OPT( 	colormode, 		OPT_DRAWCOLOR_24BIT);
	SET_OPT( 	draw_static, 	OPT_YES);
}

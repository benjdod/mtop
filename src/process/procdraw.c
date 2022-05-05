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

#include "procdraw.h"
#include "proc.h"

size_t pd_drawto(procinfo_t* p, char* buf, size_t n) {
//    printf("pd drawing to %p\n", buf);

    char* cmd_str = (*(p->cmd) == '\0')
        ? "-"
        : p->cmd;

    proc_cpuavg_t cpuavg = p->cpuavg;
    int written = snprintf(
		buf,
        n,
		"%s (%d) %c %llu %llu %llu %llu %.2f", 
		cmd_str, 
		p->pid,
		p->state,
		cpuavg.utime.current,
		cpuavg.stime.current,
		cpuavg.cutime.current,
		cpuavg.cstime.current,
        p->cpu_pct
	);

    return (written >= n) ? n : ((size_t) written);
}

static void truncate_buffer(char* buf, size_t w, size_t n) {

    if (w >= n) {
        size_t i = n-1;

        while (i && buf[i] != ' ') {
            buf[i] = ' ';
            i -= 1;
        }
    } else {
        while (w < n) {
            buf[w++] = ' ';
        }
    }
    
    return;
}

size_t pd_drawinfo(procinfo_t* p, char* buf, size_t n, u8 section) {

    size_t w = 0;

#define CLIP_BUF() 

    proc_cpuavg_t ct = p->cpuavg;

    switch (section) {
        case 0:
            //w = snprintf(buf, n, "%s - %s (%d) %c (%s)", p->user, p->cmd, p->pid, p->state, proc_state_tostring(p->state));
			w += snprintf(buf, n + w, "%s (%d) [%c] ", p->user, p->pid, p->state);
			if (p->args.argc > 0 && w < n) {
				w += x_snprintarray(buf + w, n - w, p->args.argv, p->args.argc, " ");
			} else {
				w += snprintf(buf + w, n - w, "(nil)");
			}
            break;
        case 1:
            w = snprintf(buf, n, "%llu, %llu, %llu, %llu - clock ticks of execution (user, sys, cu, cs)", ct.utime.current, ct.stime.current, ct.cutime.current, ct.cstime.current);
            break;
        case 2:
            w = snprintf(buf, n, "%lu %lu %lu %.2f - memory and cpu (res, shared, vsz, cpu %%)", p->res_mem, p->shr_mem, p->virt_mem, p->cpu_pct);
            break;
        default:
            return 0;
    }

    truncate_buffer(buf, w, n);
    return n;

}

size_t pd_drawcpuinfo(cpuinfo_t cpuinfo, char* buf, size_t n, u8 section) {
    size_t w = 0;

    switch (section) {
        case 0:
            w = snprintf(buf, n, "%llu - idle cpu!", cpuinfo.times.idle.current);
            break;
        default: 
            return 0;
    }

    /* if (w >= n) {
        //size_t i = w-1; 
        w -= 1;
        while(buf[w] != ' ' && w >= n) buf[w--] = ' ';
    } */

    truncate_buffer(buf, w, n);
    return n;
}

void pd_updatecache(procinfo_t* p) {
   // printf("updating cache at %p\n", p->drawdata.cache);
    p->drawdata.length = pd_drawto(p, p->drawdata.cache, DRAWDATA_CACHE_LENGTH);
}

int pd_get_interval(rand_hashdata_t hashdata, size_t index) {

	#define PSEUDORAND_MIN 25
	#define PSEUDORAND_MAX 60
	#define PSEUDORAND_WIDTH (PSEUDORAND_MAX - PSEUDORAND_MIN)
	#define FNV_OFFSET_BASIS 0xcbf29ce484222325
	#define FNV_PRIME 0x100000001b3

	size_t hash = FNV_OFFSET_BASIS;

	size_t input = (hashdata.base + hashdata.salt + index + 1) * (index + 1);

	while (input) {
		hash *= FNV_PRIME;
		hash ^= (input & 0xF);
		input = input >> 4;
	}

	return PSEUDORAND_MIN + hash % PSEUDORAND_WIDTH;
}

rand_hashdata_t pd_init_hashdata(pid_t pid) {
	rand_hashdata_t hd =  {
		pid, 	// base
		0 		// salt
	};
	return hd;
}

rand_drawctx_t pd_init_drawctx(pid_t pid) {
	rand_drawctx_t ctx;
	ctx.hashdata = pd_init_hashdata(pid);
	ctx.offset = 0;
	ctx.index = 0;
	ctx.rand = pd_get_interval(ctx.hashdata, ctx.index);
	return ctx;
}

void pd_advance_drawctx_interval(rand_drawctx_t* ctx) {
	ctx->index += 1;
	ctx->rand = pd_get_interval(ctx->hashdata, ctx->index);
	ctx->offset = 0;
	ctx->visible = (ctx->visible) ? 0 : 1;
}

void pd_advance_drawctx(rand_drawctx_t* ctx) {
	ctx->offset += 1;
	if (ctx->offset >= ctx->rand) {
		pd_advance_drawctx_interval(ctx);
	}
}

void pd_retract_drawctx_interval(rand_drawctx_t* ctx) {
	ctx->index -= 1;
	ctx->rand = pd_get_interval(ctx->hashdata, ctx->index);
	ctx->offset = ctx->rand - 1;
	ctx->visible = ! ctx->visible;
}

void pd_retract_drawctx(rand_drawctx_t* ctx) {
	if (ctx->offset == 0) {
		pd_retract_drawctx_interval(ctx);
	} else {
		ctx->offset -= 1;
	}
}

void pd_random_drawctx(rand_drawctx_t* ctx) {
	while (rand() < RAND_MAX / 3) {
		pd_advance_drawctx_interval(ctx);
	}
	ctx->offset = (rand() + ctx->hashdata.base) % ctx->rand;
}

inline int randd_visible(rand_drawctx_t ctx, size_t screen_offset) {
    int width = ctx.rand - ctx.offset;
	int visible = ctx.visible;
	int index = ctx.index;
	while (width <= screen_offset) {	// off by one?
		width += pd_get_interval(ctx.hashdata, ++index);
        visible = !visible;
	}
    return visible;
}

inline char pd_charat(procinfo_t* p, size_t screen_offset) {
#define DRAWCACHE_PADDING 1

    size_t final_idx = ((screen_offset + p->drawdata.offset) % (p->drawdata.length + DRAWCACHE_PADDING));

    /* If the final index is within the bounds of the draw cache and is visible according to 
     * the masking algorithm, return it. Otherwise return a space */
	return ( final_idx < p->drawdata.length && randd_visible(p->drawdata.ctx, screen_offset))
        ? p->drawdata.cache[final_idx]
        : ' ';
}

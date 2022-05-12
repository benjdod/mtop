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

#ifndef MTOP_DISPLAY_DRAWBUFFER_H
#define MTOP_DISPLAY_DRAWBUFFER_H

#include "common.h"
#include "dstring.h"
#include "dcolor.h"
#include "xutil.h"

#define DITEM_DCOLOR     0x0    // dcolor_t
#define DITEM_DSTRING   0x1     // dstring_t

typedef struct drawitem_t_ {
	// index of the item in the respective memory buffer
    u64 idx;
	// length of the item (only applies to strings)
	u64 length;
	// type of the item (string | color)
    u8 type;
} drawitem_t;

generic_buffer_typedef(dcolor_t, colorbuffer);
generic_buffer_typedef(drawitem_t, drawitembuffer);
generic_buffer_typedef(char, charbuffer);

typedef struct drawbuffer_t_ {

	charbuffer string_buffer;
	colorbuffer color_buffer;

	// this contains drawitems which are references to 
	// items in string or color buffers respectively
	drawitembuffer drawitem_buffer;

    u32 checksum;

} drawbuffer_t;

/*  TODO: implement cache? Uses checksum field to invalidate cache
typedef struct {
    drawbuffer_t* generator;
    char* cache;
    u32 checksum;
} drawbuffer_cache_t; */

drawbuffer_t dbuf_init();
size_t dbuf_renderto(drawbuffer_t* dbuf, char* dest, size_t n);
size_t dbuf_flushto(drawbuffer_t* dbuf, char* dest, size_t n);
size_t dbuf_draw(drawbuffer_t* dbuf);
void dbuf_clear(drawbuffer_t* dbuf);
void dbuf_flush(drawbuffer_t* dbuf);

void dbuf_addcolor(drawbuffer_t* dbuf, dcolor_t color);
void dbuf_adds(drawbuffer_t* dbuf, const char* str);
void dbuf_addsn(drawbuffer_t* dbuf, const char* str, size_t n);
void dbuf_addc(drawbuffer_t* dbuf, char c);
void dbuf_addcn(drawbuffer_t* dbuf, char c, size_t n);

#endif

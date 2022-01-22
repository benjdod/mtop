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

#define DITEM_DCOLOR     0x0    // dcolor_t
#define DITEM_DSTRING   0x1     // dstring_t

typedef struct {
    u32 idx;
    u8 type;
} drawitem_t;

typedef struct {

    // string buffer
    char* chbuf;
    u32 chbuf_length;
    u32 chbuf_size;

    // color buffer
    dcolor_t* colorbuf;
    u32 colorbuf_length;
    u32 colorbuf_size;

    // drawstring buffer
    dstring_t* dstrbuf;
    u32 dstrbuf_length;
    u32 dstrbuf_size;

    // top level item buffer 
    drawitem_t* buffer;
    u32 length;
    u32 size;

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

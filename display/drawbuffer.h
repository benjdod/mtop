#ifndef _CMTOP_DISPLAY_DRAWBUFFER_H
#define _CMTOP_DISPLAY_DRAWBUFFER_H

#include "common.h"
#include "dstring.h"
#include "dcolor.h"

#define DITEM_DCOLOR     0x0    // dcolor_t
#define DITEM_DSTRING   0x1     // dstring_t

typedef struct {
    void* data;
    u8 type;
} drawitem_t;

typedef struct {
    dcolor_t* colorbuf;
    u32 colorbuf_length;
    u32 colorbuf_size;
    dstring_t* dstrbuf;
    u32 dstrbuf_length;
    u32 dstrbuf_size;
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

size_t dbuf_renderto(drawbuffer_t* dbuf, char* dest, size_t n);
size_t dbuf_draw(drawbuffer_t* dbuf);
void dbuf_addcolor(drawbuffer_t* dbuf, dcolor_t color);
void dbuf_addstr(drawbuffer_t* dbuf, char* str);
void dbuf_addstrn(drawbuffer_t* dbuf, char* str, size_t n);

#endif
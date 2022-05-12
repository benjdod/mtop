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

#include "drawbuffer.h"
#include "xutil.h"
#include "tty.h"

#define BUF_EXPAND(buf, type, len, size, ext) \
    while ((len) + (ext) >= (size)) { \
        (size) *= 1.5; \
        (buf) = x_realloc((buf), (size), sizeof(type)); \
    }

drawbuffer_t dbuf_init() {
    drawbuffer_t dbuf;
	generic_buffer_init(dbuf.color_buffer, dcolor_t);
	generic_buffer_init(dbuf.string_buffer, char);
	generic_buffer_init(dbuf.drawitem_buffer, drawitem_t);
    dbuf.checksum = 0;
    return dbuf;
}

static void dbuf_additem(drawbuffer_t* dbuf, drawitem_t item) {
	generic_buffer_insert(dbuf->drawitem_buffer, item);
    dbuf->checksum += 1;
}

void dbuf_addcolor(drawbuffer_t* dbuf, dcolor_t color) {
	drawitem_t i;
	i.idx = generic_buffer_length(dbuf->color_buffer);
	i.type = DITEM_DCOLOR;
	i.length = 0;
	dbuf_additem(dbuf, i);

	generic_buffer_insert(dbuf->color_buffer, color);
}

static void dbuf_push_string(drawbuffer_t* dbuf, u64 index, size_t length) {
	if (
            generic_buffer_lastp(dbuf->drawitem_buffer) != NULL &&
            generic_buffer_lastp(dbuf->drawitem_buffer)->type == DITEM_DSTRING
    )
	{
		// if last item is a string, just expand its length to encompass 
		// extra characters we already added
		drawitem_t* last_item = generic_buffer_lastp(dbuf->drawitem_buffer);
		last_item->length += length;
	} else {
		// otherwise, append a new string item
		drawitem_t i;
		i.type = DITEM_DSTRING;
		i.length = length;
		i.idx = index;
		dbuf_additem(dbuf, i);
	}
}

void dbuf_addsn(drawbuffer_t* dbuf, const char* str, size_t n) {
	char* new_string;
	generic_buffer_insert_np(dbuf->string_buffer, n, str, new_string);
	u64 new_index = (u64) (new_string - generic_buffer_firstp(dbuf->string_buffer));
	dbuf_push_string(dbuf, new_index, n);
}

void dbuf_adds(drawbuffer_t* dbuf, const char* str) {
    size_t len = x_strlen(str);
    dbuf_addsn(dbuf, str, len);
}

void dbuf_addc(drawbuffer_t* dbuf, char c) {
	return dbuf_addcn(dbuf, c, 1);
}

void dbuf_addcn(drawbuffer_t* dbuf, char c, size_t n) {
	char* new_string;
	generic_buffer_insert_nrp(dbuf->string_buffer, n, c, new_string);
	dbuf_push_string(
		dbuf, 
		(u64) new_string - generic_buffer_first(dbuf->string_buffer), 
		n
	);
}

size_t dbuf_renderto(drawbuffer_t* dbuf, char* dest, size_t n) {

    size_t
        i_item = 0,
        i_write = 0;

    char cbuf[20];

	u64 raw_buffer_length = generic_buffer_length(dbuf->drawitem_buffer);
    
    while (1) {
        if (i_item >= raw_buffer_length) break;

        drawitem_t item = generic_buffer_firstp(dbuf->drawitem_buffer)[i_item];

        size_t bytes_left = (n > 0) ? n - i_write : SIZE_MAX;
        size_t writelen = 0;

        if (item.type == DITEM_DSTRING) {
            char* writing_string = generic_buffer_p_at(dbuf->string_buffer, item.idx);
            writelen = X_MIN(item.length, bytes_left);
            x_strncpy(dest + i_write, writing_string, writelen);
            i_write += writelen;
        } else if (item.type == DITEM_DCOLOR) {
            x_memset(cbuf, '\0', 20);
            dcolor_t* color = generic_buffer_p_at(dbuf->color_buffer, item.idx);
            size_t w = dcolor_write(*color, cbuf, 20);
            writelen = X_MIN(w, bytes_left);
            x_strncpy(dest + i_write, cbuf, writelen);
            i_write += writelen;
        }

        i_item++;
    }

    return i_write;
}

size_t dbuf_draw(drawbuffer_t* dbuf) {
    
    size_t
        i_item = 0,
        i_write = 0;

    char cbuf[20];

    while (i_item < generic_buffer_length(dbuf->drawitem_buffer)) {

		drawitem_t item = *(generic_buffer_p_at(dbuf->drawitem_buffer, i_item));

        if (item.type == DITEM_DSTRING) {
			char* string = generic_buffer_p_at(dbuf->string_buffer, item.idx);
			u64 length = item.length;
            tty_writesn(string, length);
            i_write += length;
        } else if (item.type == DITEM_DCOLOR) {
            x_memset(cbuf, '\0', 20);
			dcolor_t color = generic_buffer_firstp(dbuf->color_buffer)[item.idx];
            size_t w = dcolor_write(color, cbuf, 20);
            tty_writesn(cbuf, w);
            i_write += w;
        }

        i_item++;
    }

    return i_write;
}

void dbuf_clear(drawbuffer_t* dbuf) {
	generic_buffer_clear(dbuf->color_buffer);
	generic_buffer_clear(dbuf->drawitem_buffer);
	generic_buffer_clear(dbuf->string_buffer);
    return;
}

void dbuf_flush(drawbuffer_t* dbuf) {
    dbuf_draw(dbuf);
    dbuf_clear(dbuf);
}

size_t dbuf_flushto(drawbuffer_t* dbuf, char* dest, size_t n) {
    size_t out = dbuf_renderto(dbuf, dest, n);
    dbuf_clear(dbuf);
    return out;
}

#undef BUF_EXPAND
#undef LAST_ITEM

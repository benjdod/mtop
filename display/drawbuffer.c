#include "drawbuffer.h"
#include "xutil.h"
#include "tty.h"

#define BUF_EXPAND(buf, type, len, size) \
    if ((len) == (size)) { \
        (size) *= 1.5; \
        (buf) = realloc((buf), (size) * sizeof(type)); \
    }

static void dbuf_additem(drawbuffer_t* dbuf, drawitem_t item) {
    BUF_EXPAND(dbuf->buffer, drawitem_t, dbuf->length, dbuf->size);
    dbuf->buffer[dbuf->length++] = item;
    dbuf->checksum += 1;
}


static void dbuf_add_dstr(drawbuffer_t* dbuf, dstring_t dstr) {
    BUF_EXPAND(dbuf->dstrbuf, drawitem_t, dbuf->dstrbuf_length, dbuf->dstrbuf_size);
    dbuf->dstrbuf[dbuf->dstrbuf_length] = dstr;
    dbuf_additem(dbuf, (drawitem_t) {
        (void*) dbuf->dstrbuf + dbuf->dstrbuf_length,
        DITEM_DSTRING
    });
    dbuf->dstrbuf_length += 1;
}

void dbuf_addcolor(drawbuffer_t* dbuf, dcolor_t color) {
    BUF_EXPAND(dbuf->colorbuf, dcolor_t, dbuf->colorbuf_length, dbuf->colorbuf_size);
    dbuf->colorbuf[dbuf->colorbuf_length] = color;
    dbuf_additem(dbuf, (drawitem_t) {
        (void*) dbuf->colorbuf + dbuf->colorbuf_length,
        DITEM_DCOLOR 
    });
    dbuf->colorbuf_length += 1;
}


void dbuf_addstr(drawbuffer_t* dbuf, char* str) {
    size_t len = x_strlen(str);
    dbuf_add_dstr(dbuf, (dstring_t) {str, len});
}

void dbuf_addstrn(drawbuffer_t* dbuf, char* str, size_t n) {
    dbuf_add_dstr(dbuf, (dstring_t) {str, n});
}


size_t dbuf_renderto(drawbuffer_t* dbuf, char* dest, size_t n) {

    size_t
        i_item = 0,
        i_write = 0;

    char cbuf[20];
    
    while (1) {
        if (i_item >= dbuf->length) break;

        drawitem_t item = dbuf->buffer[i_item];

        size_t bytes_left = (n > 0) ? n - i_write : SIZE_MAX;
        size_t writelen = 0;

        if (item.type == DITEM_DSTRING) {
            dstring_t* data = (dstring_t*) item.data;
            writelen = MMIN(data->len, bytes_left);
            x_strncpy(dest + i_write, data->start, writelen);
            i_write += writelen;
            i_item++;
        } else if (item.type == DITEM_DCOLOR) {
            x_memset(cbuf, '\0', 20);
            dcolor_t* color = (dcolor_t*) item.data;
            size_t w = dcolor_write(*color, cbuf, 20);
            writelen = MMIN(w, bytes_left);
            x_strncpy(dest + i_write, cbuf, writelen);
            i_write += writelen;
            i_item++;
        }
    }

    return i_write;
}


size_t dbuf_draw(drawbuffer_t* dbuf) {
    
    size_t
        i_item = 0,
        i_write = 0;

    char cbuf[20];
    
    while (1) {
        if (i_item >= dbuf->length) break;

        drawitem_t item = dbuf->buffer[i_item];

        size_t writelen = 0;

        if (item.type == DITEM_DSTRING) {
            dstring_t* data = (dstring_t*) item.data;
            tty_writesn(data->start, data->len);
            i_write += data->len;
            i_item++;
        } else if (item.type == DITEM_DCOLOR) {
            x_memset(cbuf, '\0', 20);
            dcolor_t* color = (dcolor_t*) item.data;
            size_t w = dcolor_write(*color, cbuf, 20);
            tty_writesn(cbuf, w);
            i_write += w;
            i_item++;
        }
    }

    return i_write;
}
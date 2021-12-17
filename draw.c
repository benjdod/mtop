#include "draw.h"
#include "proc.h"
#include "procdraw.h"

size_t draw_queryrow(procs_info_t* info, char* buf, size_t n, size_t r_off, size_t c_off, int step) {

    size_t i = 0;

    if (step < 1) step = 1;

    procbst_cursor_t cur = procbst_cursor_init(&info->procs);
    procbst_cursor_next(&cur);

    for (size_t j = 0; j < c_off; j++) { procbst_cursor_next(&cur); }

    while (cur.current != NULL && i < n) {
        buf[i*step] = pd_charat(&(cur.current->value), r_off);
        procbst_cursor_next(&cur);
        i++;
    }

    return i*step;
}

size_t draw_queryscr(procs_info_t* info, char* scrbuf, size_t r_size, size_t c_size, size_t r_off, size_t c_off, int step) {
    size_t i = 0;

    if (step < 1) step = 1;

    procbst_cursor_t cur = procbst_cursor_init(&info->procs);
    procbst_cursor_next(&cur);

    for (size_t j = 0; j < c_off; j++) { procbst_cursor_next(&cur); }

    while (cur.current != NULL && i < c_size) {

        for (size_t k = 0; k < r_size * c_size; k++) {
            scrbuf[k * c_size + i*step] = pd_charat(&(cur.current->value), r_off);
        }
        procbst_cursor_next(&cur);
        i++;
    }
}
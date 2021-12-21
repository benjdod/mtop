#ifndef _CMTOP_DISPLAY_DSTRING_H
#define _CMTOP_DISPLAY_DSTRING_H

#include "common.h"

typedef struct dstring_t_ {
    char* start;
    size_t len;
} dstring_t;

dstring_t dstring_make(char* start, size_t len);
void dstring_cpy(char* dest, dstring_t src);
void dstring_ncpy(char* dest, dstring_t src, size_t n);

#endif
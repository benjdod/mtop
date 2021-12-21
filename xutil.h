#ifndef _CMTOP_XUTIL_H
#define _CMTOP_XUTIL_H

#include "common.h"

ssize_t x_readfile(const char* path, char* buf, ssize_t n);
int x_streq(const char* a, const char* b);
int x_strcmp(const char* a, const char* b);
size_t x_strlen(const char* s);
char* x_strncpy(char* dest, const char* src, size_t n);
void* x_memset(void* s, int c, size_t n);

#define X_IEPS(val, eps) (((val) < (eps)) ? 0 : (val))
#define X_FEPS(val, eps) (((val) < (eps)) ? 0.0F : (val))
#define MMIN(a,b) ((a) < (b) ? (a) : (b))
#define MMAX(a,b) ((a) > (b) ? (a) : (b))

#endif
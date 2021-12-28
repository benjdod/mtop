#ifndef _CMTOP_XUTIL_H
#define _CMTOP_XUTIL_H

#include "common.h"

#include <string.h>
#include <stdlib.h>

ssize_t x_readfile(const char* path, char* buf, ssize_t n);

// strings 
int x_streq(const char* a, const char* b);
char* x_strstr(const char* haystack, const char* needle);
int x_strcmp(const char* a, const char* b);
int x_strncmp(const char* a, const char* b, size_t n);
size_t x_strlen(const char* s);
char* x_strncpy(char* dest, const char* src, size_t n);
void* x_memset(void* s, int c, size_t n);

// memory allocation

/* allocate n members of size s */
void* x_malloc(size_t n, size_t s);

/* free memory and return NULL. 
 * good for memory safety! (ptr = x_free(ptr)) */
void* x_free(void* ptr);

/* allocate n members of size s and set to 0 */
void* x_calloc(size_t n, size_t s);

/* allocate n members of size s and set to c */
void* x_cvalloc(size_t n, size_t s, int c);

/* realloc in array format */
void* x_realloc(void* ptr, size_t n, size_t s);

#define X_IEPS(val, eps) (((val) < (eps)) ? 0 : (val))
#define X_FEPS(val, eps) (((val) < (eps)) ? 0.0F : (val))
#define X_MIN(a,b) ((a) < (b) ? (a) : (b))
#define X_MAX(a,b) ((a) > (b) ? (a) : (b))

#endif
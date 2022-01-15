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

#ifndef MTOP_XUTIL_H
#define MTOP_XUTIL_H

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
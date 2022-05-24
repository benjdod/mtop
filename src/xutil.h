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

ssize_t x_readfile(const char* path, char* buf, ssize_t n);

// strings 
int x_streq(const char* a, const char* b);
char* x_strstr(const char* haystack, const char* needle);
int x_strcmp(const char* a, const char* b);
int x_strncmp(const char* a, const char* b, size_t n);
size_t x_strlen(const char* s);
char* x_strncpy(char* dest, const char* src, size_t n);
void* x_memset(void* s, int c, size_t n);
size_t x_snprintarray(char* dest, size_t n, char** strings, size_t num_strings, const char* delimiter);

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

// A cheeky little generic buffer. Good for most ailments

#define generic_buffer_typedef(T, NAME) typedef struct {u32 idx, capacity, length; u8 elt_size; T* head;} NAME 

#define generic_buffer_init(BUF, T) {\
	BUF.length = 0; \
	BUF.elt_size = sizeof(T); \
	BUF.capacity = 8; \
	BUF.head = (T*) x_malloc(BUF.capacity, sizeof(T)); }

#define generic_buffer_destroy(BUF) (BUF->head = x_free(BUF->head))

/** expand BUF to accomodate N more elements */
#define generic_buffer_expand(BUF, N) \
	while ((BUF.length + N) > BUF.capacity) { \
		BUF.capacity *= 1.5; \
		BUF.head = x_realloc(BUF.head, BUF.capacity, BUF.elt_size); \
	}

/** insert ITEM to BUF */
#define generic_buffer_insert(BUF, ITEM) \
	generic_buffer_expand(BUF, 1); \
	BUF.head[BUF.length++] = ITEM;

/** insert an array of ITEMS of length N */
#define generic_buffer_insert_n(BUF, N, ITEMS) \
	generic_buffer_expand(BUF, N); \
	for (BUF.idx = 0; BUF.idx < N; BUF.idx += 1) { \
		BUF.head[BUF.length + BUF.idx] = ITEMS[BUF.idx]; \
	} \
	BUF.idx = 0; \
	BUF.length += N;

/** insert an array of ITEMS of length N and return the front of the 
 * newly inserted array in RET_PTR */
#define generic_buffer_insert_np(BUF, N, ITEMS, RET_PTR) \
	generic_buffer_expand(BUF, N); \
	for (BUF.idx = 0; BUF.idx < N; BUF.idx += 1) { \
		BUF.head[BUF.length + BUF.idx] = ITEMS[BUF.idx]; \
	} \
	BUF.idx = 0; \
	RET_PTR = &(BUF.head[BUF.length]); \
	BUF.length += N;

/** insert ITEM to BUF N times */
#define generic_buffer_insert_nr(BUF, N, ITEM) \
	generic_buffer_expand(BUF,N); \
	for (BUF.idx = 0; BUF.idx < N; BUF.idx += 1) { \
		BUF.head[BUF.length + BUF.idx] = ITEM; \
	} \
	BUF.idx = 0; \
	BUF.length += N;

/** insert ITEM N times and return the front of the newly inserted 
 * array in RET_PTR */
#define generic_buffer_insert_nrp(BUF, N, ITEM, RET_PTR) \
	generic_buffer_expand(BUF,N); \
	for (BUF.idx = 0; BUF.idx < N; BUF.idx += 1) { \
		BUF.head[BUF.length + BUF.idx] = ITEM; \
	} \
	BUF.idx = 0; \
	RET_PTR = &(BUF.head[BUF.length]); \
	BUF.length += N;

/** repeatedly insert an array of ITEMS of length N R times */
#define generic_buffer_insert_nnr(BUF, N, ITEMS, R) \
	generic_buffer_expand(BUF, N * R); \
	for (BUF.idx = 0; BUF.idx < N * R; BUF.idx += 1) { \
		BUF.head[BUF.length + BUF.idx] = ITEMS[BUF.idx % N]; \
	} \
	BUF.idx = 0; \
	BUF.length += N*R;

/** remove N items from BUF */
#define generic_buffer_remove(BUF, N) BUF.length -= N;

/** clear all items from BUF */
#define generic_buffer_clear(BUF) \
	BUF.length = 0; \

/** get the length (# of items) of BUF */
#define generic_buffer_length(BUF) (BUF.length)
#define generic_buffer_firstp(BUF) (BUF.head)
#define generic_buffer_lastp(BUF) ((BUF.length > 0 ) ? &(BUF.head[BUF.length - 1]) : NULL)
#define generic_buffer_first(BUF) (BUF.head[0])
#define generic_buffer_last(BUF) (BUF.head[BUF.length - 1])

/** get a pointer to the value stored at index I in BUF, returning NULL if the index is out of range */
#define generic_buffer_p_at(BUF, I) ((I < BUF.length && I >= 0) ? &(BUF.head[I]) : NULL)

#endif

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

#include "xutil.h"

ssize_t x_readfile(const char* path, char* buf, ssize_t n) {
    FILE* fptr = fopen(path, "r");

    if (fptr == NULL) return -1;

    ssize_t n_read = (ssize_t) fread(buf, sizeof(char), n, fptr); 

    if (fclose(fptr)) {
        ; // do something here? we don't really care if it doesn't close...
    }

    return n_read;
}

ssize_t x_writefile(const char* path, const char* data) {
    FILE* fptr = fopen(path, "a");
    if (fptr == NULL) return -1;
    size_t nmemb = x_strlen(data);
    size_t w = fwrite(data, sizeof(char), nmemb, fptr);
    fclose(fptr);
    return (ssize_t) w;
}

int x_streq(const char* a, const char* b) {
    return strcmp(a,b) == 0 ? 1 : 0;
}

size_t x_strlen(const char* s) {
    return strlen(s);
}

int x_strcmp(const char* a, const char* b) {
    return strcmp(a,b);
}

char* x_strstr(const char* haystack, const char* needle) {
    return strstr(haystack, needle);
}

int x_strncmp(const char* a, const char* b, size_t n) {
    return strncmp(a,b, n);
}

char* x_strncpy(char* dest, const char* src, size_t n) {
    return strncpy(dest, src, n);
}

void* x_memset(void* s, int c, size_t n) {
    return memset(s,c,n);
}

/**
 * Print an array of strings seperated by a delimiter to a buffer.
 * If any char * in the array is NULL it will be skipped.
 * */
size_t x_snprintarray(char* dest, size_t n, char** strings, size_t num_strings, const char* delimiter) {

    if (num_strings == 0 || strings == NULL) return 0;

    size_t delim_length = x_strlen(delimiter);
    size_t offset = 0;
    size_t copy_bytes = 0;

    for (size_t i = 0; i < num_strings && offset < n; i++) {
        if (strings[i] == NULL) continue;
        if (offset < n) {
            size_t arg_length = x_strlen(strings[i]);
            copy_bytes = X_MIN(arg_length, n - offset);
            x_strncpy(&dest[offset], strings[i], copy_bytes);
            offset += copy_bytes;
        } 
        if (offset < n) {
            copy_bytes = X_MIN(delim_length, n - offset);
            x_strncpy(&dest[offset], delimiter, copy_bytes);
            offset += copy_bytes;
        }
    }

    if (offset == 0) dest[offset++] = ' '; 

    return offset;
}


// memory functions

void* x_malloc(size_t n, size_t s) {
    return malloc(n*s);
}

void* x_free(void* ptr) {
    free(ptr);
    return NULL;
}

void* x_calloc(size_t n, size_t s) {
    return calloc(n, s);
}

void* x_cvalloc(size_t n, size_t s, int c) {
    void* ptr = x_malloc(n, s);
    x_memset(ptr, c, n * s);
    return ptr;
}

void* x_realloc(void* ptr, size_t n, size_t s) {
    return realloc(ptr, s * n);
}

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

// memory functions

void* x_malloc(size_t n, size_t s) {

    /*
    void *array[2];
    char **strings;
    int size = 0;

    size = backtrace(array, 2);
    strings = backtrace_symbols(array, 2);


    char buf[8192];
    int offset = 0;
    x_memset(buf, '\0', 8192);

    void* out = malloc(n * s);

    if (strings != NULL) {
        offset += snprintf(&buf[offset], 8192 - offset, "allocated %lu at %p\n", n*s, out);
        for (int i = 0; i < size; i++) {
            offset += snprintf(&buf[offset], 8192 - offset, "at %s\n", strings[i]);
        }
    } else {
        snprintf(buf, 8192, "??? allocated %lu at %p\n", n * s, out);
    }

    x_writefile("./malloc.log", buf);
    
    return out;
    */

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
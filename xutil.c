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

int x_streq(const char* a, const char* b) {
    return strcmp(a,b) == 0 ? 1 : 0;
}

int x_strcmp(const char* a, const char* b) {
    return strcmp(a,b);
}

char* x_strncpy(char* dest, const char* src, size_t n) {
    return strncpy(dest, src, n);
}

void* x_memset(void* s, int c, size_t n) {
    return memset(s,c,n);
}
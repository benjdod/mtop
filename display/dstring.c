#include "dstring.h"
#include "xutil.h"

dstring_t dstring_make(char* start, size_t len) {
    return (dstring_t) {start, len};
}

void dstring_cpy(char* dest, dstring_t src) {
    x_strncpy(dest, src.start, src.len);
}

void dstring_ncpy(char* dest, dstring_t src, size_t n) {
    x_strncpy(dest, src.start, MMIN(src.len, n));
}
#ifndef MTOP_DMETER_H
#define MTOP_DMETER_H

#include "common.h"

typedef struct dmeter_t_ {
    u16 
        min,
        max,  
        value,
        width;
} dmeter_t;

size_t dmeter_draw(dmeter_t dm, char* buf, size_t n);

#endif
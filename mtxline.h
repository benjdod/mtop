#ifndef _MTXLINE_H
#define _MTXLINE_H

#include "proc.h"

typedef struct {
	int32_t offset;
	uint16_t length;
	uint16_t speed;
} mtxline_t;

typedef struct {
	mtxline_t *lines;
	size_t count;
} mtxlines_t;

int mtx_init_lines(size_t n, size_t length);
mtxline_t mtx_make_line(uint16_t length, uint16_t speed);
size_t mtx_query_row(uint16_t, char *, size_t);
void mtx_step_lines();

#endif

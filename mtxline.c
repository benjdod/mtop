#include "common.h"
#include "mtxline.h"
#include "proc.h"

mtxlines_t lines;

int mtx_init_lines(size_t n, size_t length) {
	lines.lines = (mtxline_t*) malloc(n * sizeof(mtxline_t));
	if (lines.lines == NULL) return 1;
	lines.count = n;

	size_t min_length = 5;
	size_t max_length = length - 5;
	size_t offset_delta = 20;

	for (size_t i = 0; i < lines.count; i++) {
		mtxline_t l;
		l.length = min_length + rand() % (max_length - min_length);
		l.offset = 0 - l.length - (rand() % (offset_delta));
		l.speed = 1;
		lines.lines[i] = mtx_make_line(length, 1);
	}

	return 0;
}

mtxline_t mtx_make_line(uint16_t length, uint16_t speed) {
	mtxline_t l;

	size_t offset_delta = 20;
	size_t min_length = 5;
	size_t max_length = length - 5;

	l.length = min_length + rand() % (max_length - min_length);
	l.offset = 0 - l.length - (rand() % (offset_delta));
	l.speed = speed;
}

size_t mtx_query_row(uint16_t row, char *buffer, size_t buffer_limit) {
	size_t true_limit = (lines.count > buffer_limit) 
		? buffer_limit 
		: lines.count;

	for (size_t i = 0; i < true_limit; i++) {
		mtxline_t l = lines.lines[i];

		if (
				row >= l.offset &&
				row < (l.offset + l.length) 
		   ) {
			buffer[i] = '$';
		} else {
			buffer[i] = ' ';
		}
	}

	return true_limit;
}

void mtx_step_lines() {
	for (size_t i = 0; i < lines.count; i++) {
		lines.lines[i].offset += 1;
	}
}

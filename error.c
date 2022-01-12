#include "common.h"

#include "error.h"
#include "tty.h"

void fatal(int e_no, const char *msg, ...) {

	va_list args;
	va_start(args,msg);

	if (!tty_israw()) {
		printf("Fatal error (code %d): ",e_no);
		vprintf(msg,args);
		putchar('\n');
		va_end(args);
		return;
	}

	tty_clear();

	char cp[4096];

	size_t pos = 0;

	pos += snprintf(cp,4096,"Fatal error (code %d): ",e_no);

	pos += vsnprintf(cp+pos,4096-pos-3,msg,args);
	va_end(args);

	snprintf(cp+pos,3,"\r\n");

	tty_writes(cp);

	tty_writes("\r\nPress any key to exit...\r\n");

	char c;

	while (1) {
		ssize_t r = read(STDIN_FILENO,&c,1);

		if ((r == -1) || (r)) {
			break;
		}
	}

	tty_reset();

	exit(e_no);
}

void error(int e_no, const char *msg, ...) {
	va_list args;
	va_start(args,msg);

	if (!tty_israw()) {
		printf("Error: ");
		vprintf(msg,args);
		putchar('\n');
		va_end(args);
		exit(e_no);
	}

	tty_clear();

	char cp[4096];

	size_t pos = 0;

	pos += snprintf(cp,4096,"Error (code %d): ",e_no);
	pos += vsnprintf(cp+pos,4096-pos-3,msg,args);
	snprintf(cp+pos,3,"\r\n");
	va_end(args);

	tty_writes(cp);

	tty_writes("\r\nPress ctrl + c to exit...\n");

	char c;

	while (1) {
		ssize_t r = read(STDIN_FILENO,&c,1);
		
		if ((r == -1) || (r)) {
			break;
		}
	}

	tty_reset();

	exit(e_no);
}

void warn(char *msg) {
	fatal(1000,msg);
}

void notify(char *msg) {
	fatal(2000,msg);
}

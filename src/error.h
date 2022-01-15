#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>

void fatal(int, const char *, ...);
void error(int, const char *, ...);
void warn(char *);
void notify(char *);

#endif

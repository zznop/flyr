#ifndef _UTILS_H
#define _UTILS_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define FAILURE 1
#define SUCCESS 0

void info(const char *fmt, ...);
void err(const char *fmt, ...);

#endif

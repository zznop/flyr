#ifndef _UTILS_H
#define _UTILS_H

#include <stdarg.h>
#include <stdio.h>

#define FAILURE 1
#define SUCCESS 0

void dudinfo(const char *fmt, ...);
void duderr(const char *fmt, ...);

#endif

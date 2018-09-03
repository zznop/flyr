/*
 * utils.c
 */

#include "utils.h"
#define LOG_BUF_SIZE (256)

/**
 * Print a dudley info message to stdout
 */
void dudinfo(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);
    fprintf(stdout, "* %s\n", buf);
}

/**
 * Print a dudley error message to stdout
 */
void duderr(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);
    fprintf(stderr, "! %s\n", buf);
}

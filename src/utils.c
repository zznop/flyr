/*
 * utils.c
 */

#include "utils.h"

#define LOG_BUF_SIZE (256)

/**
 * Writes formatted strings to file descriptors
 * @param out File descriptor to write to
 * @param prefix Delimiter to prefix message with
 * @param fmt Formatted output
 */
static void dudwrite(FILE *out, const char prefix, const char *fmt, ...)
{
    va_list ap;
	char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);
    fprintf(out, "%c %s\n", prefix, buf);
}

/**
 * Print a dudley info message to stdout
 * @param fmt Format string
 */
void dudinfo(const char *fmt, ...)
{
    dudwrite(stdout, '*', fmt);
}

/**
 * Print a dudley error message to stdout
 * @param fmt Format string
 */
void duderr(const char *fmt, ...)
{
    dudwrite(stderr, '!', fmt);
}

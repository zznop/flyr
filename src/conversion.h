#ifndef _CONVERSION_H
#define _CONVERSION_H

#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <byteswap.h>
#include <stdlib.h>

typedef enum {
    LITEND = 0,
    BIGEND,
    IRREND, /* irrelevant */
    ERREND
} endianess_t;

long hexstr_to_long(const char *hexstr);
uint8_t hexstr_to_byte(const char *hexstr);
uint32_t hexstr_to_dword(const char *hexstr, endianess_t endian);
uint16_t hexstr_to_word(const char *hexstr, endianess_t endian);
uint64_t hexstr_to_qword(const char *hexstr, endianess_t endian);

#endif
#ifndef _CONVERSION_H
#define _CONVERSION_H

#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <byteswap.h>
#include <stdlib.h>

enum endianess {
    LITTLE_ENDIAN = 0,
    BIG_ENDIAN,
    ERRONEOUS_ENDIAN
};

long hex_string_to_long(const char *hexstr);
uint32_t hex_string_to_dword(const char *hexstr, enum endianess endian);
uint16_t hex_string_to_word(const char *hexstr, enum endianess endian);

#endif
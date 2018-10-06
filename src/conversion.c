/**
 * conversion.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "conversion.h"

long hex_string_to_long(const char *hexstr)
{
    long val;

    if (!hexstr)
        return -1;

    val = strtol(hexstr, NULL, 0);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
        (errno != 0 && val == 0)) {
        return -1;
    }

    return val;
}

uint32_t hex_string_to_dword(const char *hexstr, enum endianess endian)
{
    uint32_t val;

    if (!hexstr)
        return -1;

    val = strtoul(hexstr, NULL, 0);
    if ((errno == ERANGE) || (errno != 0 && val == 0)) {
        return -1;
    }

    if (endian == BIG_ENDIAN) {
        return __bswap_32(val);
    }

    return val;
}

uint16_t hex_string_to_word(const char *hexstr, enum endianess endian)
{
    uint32_t valdword;
    uint16_t valword;

    if (!hexstr)
        return -1;

    valdword = strtoul(hexstr, NULL, 0);
    if ((errno == ERANGE) || (errno != 0 && valdword == 0)) {
        return -1;
    }

    valword = (uint16_t)valdword;
    if (endian == BIG_ENDIAN) {
        return __bswap_16(valword);
    }

    return valword;
}
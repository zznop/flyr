/**
 * conversion.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdlib.h>
#include <errno.h>
#include "conversion.h"

long hexstr_to_long(const char *hexstr)
{
    if (!hexstr) {
        errno = EFAULT;
        return 0;
    }

    return strtol(hexstr, NULL, 0);
}

uint8_t hexstr_to_byte(const char *hexstr)
{
    uint32_t val;

    if (!hexstr) {
        errno = EFAULT;
        return 0;
    }

    val = strtoul(hexstr, NULL, 0);
    if (val & 0xffffff00) {
        errno = ERANGE;
        return 0;
    }

    return (uint8_t)val;
}

uint32_t hexstr_to_dword(const char *hexstr, endianess_t endian)
{
    uint32_t val;

    if (!hexstr) {
        errno = EFAULT;
        return 0;
    }

    val = strtoul(hexstr, NULL, 0);
    if (endian == BIGEND) {
        return __bswap_32(val);
    }

    return val;
}

uint16_t hexstr_to_word(const char *hexstr, endianess_t endian)
{
    uint32_t valdword;
    uint16_t valword;

    if (!hexstr) {
        errno = EFAULT;
        return 0;
    }

    valdword = strtoul(hexstr, NULL, 0);
    valword = (uint16_t)valdword;
    if (endian == BIGEND) {
        return __bswap_16(valword);
    }

    return valword;
}

uint64_t hexstr_to_qword(const char *hexstr, endianess_t endian)
{
    uint64_t qword;
    char *end;

    if (!hexstr) {
        errno = EFAULT;
        return 0;
    }

    qword = strtoull(hexstr, &end, 0);
    if (endian == BIGEND)
        return __bswap_64(qword);

    return qword;
}
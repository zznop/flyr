#ifndef _CONVERSION_H
#define _CONVERSION_H

#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <byteswap.h>
#include <stdlib.h>

//added fix to handle different OS byteswap defines
#ifndef __bswap16
#define __bswap16 __bswap_16
#endif
#ifndef __bswap32
#define __bswap32 __bswap_32
#endif
#ifndef __bswap64
#define __bswap64 __bswap_64
#endif

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

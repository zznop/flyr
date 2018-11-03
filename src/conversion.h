#ifndef _CONVERSION_H
#define _CONVERSION_H

#include <stdint.h>
#include <byteswap.h>

typedef enum {
    LITEND = 0,
    BIGEND,
    IRREND, /* irrelevant */
    ERREND
} endianess_t;

enum length_type {
    UNDEF_LENGTH_TYPE = 0,
    BYTE_LENGTH_TYPE,
    WORD_LENGTH_TYPE,
    DWORD_LENGTH_TYPE,
    QWORD_LENGTH_TYPE
};

// Required for certain Linux distros
#ifndef __bswap16
#define __bswap16 __bswap_16
#endif
#ifndef __bswap32
#define __bswap32 __bswap_32
#endif
#ifndef __bswap64
#define __bswap64 __bswap_64
#endif

long hexstr_to_long(const char *hexstr);
uint8_t hexstr_to_byte(const char *hexstr);
uint32_t hexstr_to_dword(const char *hexstr, endianess_t endian);
uint16_t hexstr_to_word(const char *hexstr, endianess_t endian);
uint64_t hexstr_to_qword(const char *hexstr, endianess_t endian);

#endif

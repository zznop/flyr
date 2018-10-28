#ifndef _FLYR_H
#define _FLYR_H

#include "conversion.h"
#include <stdint.h>
#include <stdlib.h>

enum length_type {
    UNDEF_LENGTH_TYPE = 0,
    BYTE_LENGTH_TYPE,
    WORD_LENGTH_TYPE,
    DWORD_LENGTH_TYPE,
    QWORD_LENGTH_TYPE
};

enum output_method {
    OUTPUT_UNSPECIFIED = 0,
    OUTPUT_FILEOUT
};

struct output_params {
    const char *directory_path;
    const char *name_suffix;
};

struct block_metadata {
    const char *name;
    size_t start;
    size_t size;
    struct json_array_t *length_blocks;
    struct block_metadata *next;
    enum length_type type;      /* class length blocks only */
    endianess_t endian;         /* class length blocks only */
};

typedef struct {
    int method;
    struct json_value_t *json_value;
    struct output_params *params;
} output_t;

typedef struct {
    struct json_value_t *json_value;
    size_t count;
    size_t idx;
    struct block_metadata *list;
} blocks_t;

typedef struct {
    struct json_value_t *json_value;
    size_t count;
    size_t idx;
} mutations_t;

typedef struct {
    uint8_t *data;
    size_t idx;
    size_t size;
} buffer_t;

typedef struct {
    const char *name;
    struct json_value_t *json_root;
    blocks_t *blocks;
    mutations_t *mutations;
    output_t *output;
    buffer_t buffer;
} flyr_t;

flyr_t *load_file(const char *filepath);
void destroy_context(flyr_t *ctx);

#endif
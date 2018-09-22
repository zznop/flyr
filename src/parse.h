#ifndef _PARSE_H
#define _PARSE_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "parson/parson.h"

enum output_method {
    OUTPUT_UNSPECIFIED = 0,
    OUTPUT_FILEOUT
};

struct output_params {
    const char *directory_path;
    const char *name_suffix;
};

typedef struct {
    enum output_method method;
    struct json_value_t *json_value;
    struct output_params *params;
} output_t;

typedef struct {
    struct json_value_t *json_value;
    size_t count;
    size_t idx;
} actions_t;

typedef struct {
    struct json_value_t *json_value;
    size_t count;
    size_t idx;
} mutations_t;

typedef struct {
    uint8_t *data;
    uint8_t *ptr;
    size_t size;
} buffer_t;

typedef struct {
    const char *name;
    struct json_value_t *json_root;
    actions_t *actions;
    mutations_t *mutations;
    output_t *output;
    buffer_t buffer;
} dud_t;

dud_t *load_file(const char *filepath);
void destroy_context(dud_t *ctx);

#endif

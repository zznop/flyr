#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "parson/parson.h"

enum output_method {
    OUTPUT_UNSPECIFIED = 0,
    OUTPUT_FILEOUT
};

struct output_handler {
    enum output_method method;
    struct json_value_t *json_value;
    struct output_params *params;
};

struct output_params {
    const char *directory_path;
    const char *name_suffix;
};

struct actions_handler {
    struct json_value_t *json_value;
    size_t count;
    size_t idx;
};

struct buffer_handler {
    uint8_t *data;
    uint8_t *ptr;
    size_t size;
};

typedef struct {
    const char *name;
    struct json_value_t *json_root;
    struct actions_handler *actions;
    struct output_handler *output;
    struct buffer_handler buffer;
} dud_t;

dud_t *dudley_load_file(const char *filepath);
void dudley_destroy(dud_t *ctx);
int dudley_iterate_actions(dud_t *ctx);

#endif

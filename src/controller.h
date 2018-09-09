#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "parson/parson.h"

int parse_dudley_file(const char *filepath);

enum output_method {
    OUTPUT_UNSPECIFIED = 0,
    OUTPUT_FILEOUT
};

struct output_params {
    enum output_method method;
    struct json_value_t *json_output_value;
    struct file_out_params *params;
};

struct file_out_params {
    const char *directory_path;
    const char *name_suffix;
};

#endif

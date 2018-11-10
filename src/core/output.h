#ifndef _OUTPUT_H
#define _OUTPUT_H

#include "flyr.h"

enum output_method {
    OUTPUT_UNSPECIFIED = 0,
    OUTPUT_FILEOUT
};

int output_mutated_data(flyr_t *ctx);

#endif
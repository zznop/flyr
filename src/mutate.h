#ifndef _MUTATE_H
#define _MUTATE_H

#include "flyr.h"

typedef int (*callback_t)(flyr_t *ctx);

// Eventually, we'll have more types
enum mutation_type {
    BITFLIP = 0
};

typedef struct {
    size_t start;
    size_t stop;
} bitflip_t;

int iterate_mutations(flyr_t *ctx, callback_t callback);

#endif
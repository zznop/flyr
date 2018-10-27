#ifndef _MUTATE_H
#define _MUTATE_H

#include "dudley.h"

typedef int (*callback_t)(dud_t *ctx);

// Eventually, we'll have more types
enum mutation_type {
    BITFLIP = 0
};

typedef struct {
    size_t start;
    size_t stop;
} bitflip_t;

int iterate_mutations(dud_t *ctx, callback_t callback);

#endif
#ifndef _MUTATE_H
#define _MUTATE_H

#include "parse.h"

// Eventually, we'll have more types
enum mutation_type {
	BITFLIP = 0
};

typedef struct {
	size_t start;
	size_t stop;
} bitflip_t;

int iterate_mutations(dud_t *ctx);

#endif
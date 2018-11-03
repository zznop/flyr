#ifndef _MUTATE_H
#define _MUTATE_H

#include "flyr.h"

typedef int (*callback_t)(flyr_t *ctx);
int iterate_mutations(flyr_t *ctx, callback_t callback);

#endif
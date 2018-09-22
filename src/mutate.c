#include "mutate.h"
#include "utils.h"

/**
 * Iterate and apply mutations
 */
int iterate_mutations(dud_t *ctx)
{
	struct json_value_t *mutation_json_value = NULL;
    for (ctx->mutations->idx = 0; ctx->mutations->idx < ctx->mutations->count; ctx->mutations->idx++) {
        mutation_json_value = json_object_get_value_at(
                json_object(ctx->actions->json_value), ctx->mutations->idx);
        if (!mutation_json_value) {
            duderr("Failed to retrieve next JSON mutation (idx: %lu", ctx->mutations->idx);
            return FAILURE;
        }

        // TODO: handle mutation
    }

    return SUCCESS;
}
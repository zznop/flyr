/**
 * unload.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "unload.h"
#include "parson/parson.h"

static void free_block_metadata_list(struct block_metadata *list)
{
	struct block_metadata *curr, *next;
	curr = list;

	while (curr->next != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
}

void destroy_context(flyr_t *ctx)
{
    if (ctx->blocks)
        free(ctx->blocks);

    if (ctx->output) {
        if (ctx->output->params)
            free(ctx->output->params);

        free(ctx->output);
    }

    if (ctx->blocks->list) {
    	free_block_metadata_list(ctx->blocks->list);
    }

    if (ctx->mutations)
        free(ctx->mutations);

    if (ctx->json_root)
        json_value_free(ctx->json_root);

    if (ctx->buffer.data)
        free(ctx->buffer.data);

    free(ctx);
    ctx = NULL;
}
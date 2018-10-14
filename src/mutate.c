/**
 * mutate.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "mutate.h"
#include "utils.h"
#include "conversion.h"
#include "parson/parson.h"

#define BITS_IN_BYTE (8)
#define BITFLIP(ptr, pos) \
    *ptr ^= 1UL << pos;

static int bitflip_and_invoke_callback(long start, long stop,
    dud_t *ctx, callback_t callback)
{
    size_t i, j;
    char saved;
    for (i = (size_t)start; i <= (size_t)stop; i++) {
        saved = ctx->buffer.data[i];
        for (j = 0; j < BITS_IN_BYTE; j++) {
            BITFLIP(&ctx->buffer.data[i], j);

            if (callback(ctx) != SUCCESS) {
                ctx->buffer.data[i] = saved;
                return FAILURE;
            }

            ctx->buffer.data[i] = saved;
        }
    }

    return SUCCESS;
}

static int handle_bitflip_mutation(struct json_value_t *action_json_value,
    dud_t *ctx, callback_t callback)
{
    long start_offset, stop_offset;

    start_offset = hexstr_to_long(
        json_object_get_string(json_object(action_json_value), "start")
    );

    if (start_offset == 0 && errno != 0) {
        duderr("Failed to parse JSON mutation start offset as a number");
        return FAILURE;
    }

    stop_offset = hexstr_to_long(
        json_object_get_string(json_object(action_json_value), "stop")
    );

    if (stop_offset == 0 && errno != 0) {
        duderr("Failed to parse JSON mutation stop offset as a number");
        return FAILURE;
    }

    if ((size_t)stop_offset > ctx->buffer.size) {
        duderr("Bitflip stop offset exceeds end of buffer");
        return FAILURE;
    }

    return bitflip_and_invoke_callback(start_offset, stop_offset, ctx, callback);
}

static int handle_mutation(struct json_value_t *action_json_value, dud_t *ctx, callback_t callback)
{
    const char *action = json_object_get_string(json_object(action_json_value), "action");
    if (!action) {
        duderr("Failed to parse mutation action from JSON");
        return FAILURE;
    }

    if (!strcmp(action, "bitflip"))
        return handle_bitflip_mutation(action_json_value, ctx, callback);

    duderr("Erroneous mutation action: %s", action);
    return FAILURE;
}

int iterate_mutations(dud_t *ctx, callback_t callback)
{
    struct json_value_t *action_json_value = NULL;
    for (ctx->mutations->idx = 0; ctx->mutations->idx < ctx->mutations->count; ctx->mutations->idx++) {
        action_json_value = json_object_get_value_at(
                json_object(ctx->mutations->json_value), ctx->mutations->idx);
        if (!action_json_value) {
            duderr("Failed to parse mutation json (idx: %lu", ctx->mutations->idx);
            return FAILURE;
        }

        handle_mutation(action_json_value, ctx, callback);
    }

    return SUCCESS;
}
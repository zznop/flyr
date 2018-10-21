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

static struct block_metadata *get_block_by_name(dud_t *ctx, const char *name)
{
    struct block_metadata *curr = ctx->blocks->list;
    while (curr) {
        if (!strcmp(curr->name, name)) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

static int fixup_length_block(dud_t *ctx, const char *name, size_t size)
{
    struct block_metadata *lenblock;

    if (!name)
        goto error;

    lenblock = get_block_by_name(ctx, name);
    if (!lenblock)
        goto error;

    // TODO: this could probably be done more elegantly with masks
    switch (lenblock->size) {
        case 1: {
            *(uint8_t *)lenblock->start += size;
            break;
        }
        case 2: {
            uint16_t word = *(uint16_t *)lenblock->start;
            if (lenblock->endian == LITEND) {
                word = __bswap16(word) + size;
                word = __bswap16(word);
            }

            printf("wtf: %04x\n", word);
            *(uint16_t *)lenblock->start = word;
            break;
        }

        case 4: {
            uint32_t dword = *(uint32_t *)lenblock->start;
            if (lenblock->endian == LITEND) {
                dword = __bswap32(dword) + size;
                dword = __bswap32(dword);
            }

            *(uint32_t *)lenblock->start = dword;
            break;
        }

        case 8: {
            uint64_t qword = *(uint64_t *)lenblock->start;
            if (lenblock->endian == LITEND) {
                qword = __bswap64(qword) + size;
                qword = __bswap64(qword);
            }

            *(uint64_t *)lenblock->start = qword;
            break;
        }
    }

    return SUCCESS;
error:
    duderr("Failed to find length block for fixup: %s", name);
    return FAILURE;
}

static int fixup_length_blocks(dud_t *ctx)
{
    struct block_metadata *curr;
    size_t count, i;

    if (!ctx->blocks->list) {
        duderr("Block linked list is NULL");
        return FAILURE;
    }

    curr = ctx->blocks->list;
    while (curr) {
        // Check if block is assigned length blocks
        if (!curr->length_blocks) {
            goto next;
        }

        // Get length block count
        count = json_array_get_count(curr->length_blocks);
        if (!count) {
            duderr("Failed to get length block array count");
            return FAILURE;
        }

        // Iterate and fixup length blocks
        for (i = 0; i < count; i++) {
            fixup_length_block(ctx, json_array_get_string(curr->length_blocks, i), curr->size);
        }

next:
        curr = curr->next;
    }

    return SUCCESS;
}

static int bitflip_and_invoke_callback(long start, long stop,
    dud_t *ctx, callback_t callback)
{
    size_t i, j;
    char saved;
    for (i = (size_t)start; i <= (size_t)stop; i++) {
        saved = ctx->buffer.data[i];
        for (j = 0; j < BITS_IN_BYTE; j++) {
            BITFLIP(&ctx->buffer.data[i], j);

            if (fixup_length_blocks(ctx)) {
                duderr("Failed to fixup length blocks");
                ctx->buffer.data[i] = saved;
                return FAILURE;
            }

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
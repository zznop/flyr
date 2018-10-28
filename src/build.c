/**
 * build.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "build.h"
#include "utils.h"
#include "conversion.h"
#include "parson/parson.h"
#include <endian.h>

static struct block_metadata *get_block_by_name(flyr_t *ctx, const char *name)
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

static int fixup_length_block(flyr_t *ctx, const char *name, size_t size)
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
            *(uint8_t *)(ctx->buffer.data + lenblock->start) += size;
            break;
        }

        case 2: {
            uint16_t word = *(uint16_t *)(ctx->buffer.data + lenblock->start);
            if (lenblock->endian == LITEND) {
                word = __bswap16(word) + size;
                word = __bswap16(word);
            } else {
                word += size;
            }

            *(uint16_t *)(ctx->buffer.data + lenblock->start) = word;
            word = *(uint16_t *)(ctx->buffer.data + lenblock->start);
            break;
        }

        case 4: {
            uint32_t dword = *(uint32_t *)(ctx->buffer.data + lenblock->start);
            if (lenblock->endian == LITEND) {
                dword = __bswap32(dword) + size;
                dword = __bswap32(dword);
            } else {
                dword += size;
            }

            *(uint32_t *)(ctx->buffer.data + lenblock->start) = dword;
            break;
        }

        case 8: {
            uint64_t qword = *(uint64_t *)(ctx->buffer.data + lenblock->start);
            if (lenblock->endian == LITEND) {
                qword = __bswap64(qword) + size;
                qword = __bswap64(qword);
            } else {
                qword += size;
            }

            *(uint64_t *)(ctx->buffer.data + lenblock->start) = qword;
            break;
        }
    }

    return SUCCESS;
error:
    err("Failed to find length block for fixup: %s", name);
    return FAILURE;
}

static int fixup_length_blocks(flyr_t *ctx)
{
    struct block_metadata *curr;
    size_t count, i;

    if (!ctx->blocks->list) {
        err("Block linked list is NULL");
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
            err("Failed to get length block array count");
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

// TODO: a lot of arguments are being passed to this function - need to clean this up
static int push_block(const char *name, flyr_t *ctx,
    size_t size, struct json_array_t *length_blocks, endianess_t endian)
{
    struct block_metadata *curr;
    struct block_metadata *tail;
    curr = (struct block_metadata *)malloc(sizeof(*curr));
    if (!curr) {
        err("Out of memory");
        return FAILURE;
    }

    memset(curr, '\0', sizeof(*curr));
    curr->name = name;
    curr->start = ctx->buffer.idx;
    curr->size = size;
    curr->length_blocks = length_blocks;
    curr->next = NULL;
    curr->endian = endian;

    if (!ctx->blocks->list) {
        // first element, set head
        ctx->blocks->list = curr;
    } else {
        // iterate until we get to the end and set tail->next
        tail = ctx->blocks->list;
        while (1) {
            if (!tail->next) {
                tail->next = curr;
                break;
            }

            tail = tail->next;
        }
    }

    return SUCCESS;
}

static int realloc_data_buffer(flyr_t *ctx, size_t size)
{
    uint8_t *tmp = NULL;

    if (!ctx->buffer.data) {
        ctx->buffer.data = (uint8_t *)malloc(size);
        if (!ctx->buffer.data) {
            err("Out of memory");
            return FAILURE;
        }

        ctx->buffer.idx = 0;
    } else {
        tmp = realloc(ctx->buffer.data, ctx->buffer.size + size);
        if (!ctx->buffer.data) {
            err("Out of memory");
            return FAILURE;
        }

        ctx->buffer.data = tmp;
    }

    ctx->buffer.size += size;
    return SUCCESS;
}

static int consume_hexstr(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    size_t data_size = 0;
    const char *pos = NULL;
    size_t i = 0;
    const char *value;

    value = json_object_get_string(json_object(block_json_value), "value");
    if (!value) {
        err("Failed to read data from block");
        return FAILURE;
    }

    if (value[strspn(value, "0123456789abcdefABCDEF")]) {
        err("Input data is not a valid hex string");
        return FAILURE;
    }

    data_size = strlen(value) / 2;
    if (realloc_data_buffer(ctx, data_size))
        return FAILURE;

    pos = value;
    for (i = ctx->buffer.idx; i < ctx->buffer.idx + data_size; i++) {
        sscanf(pos, "%2hhx", ctx->buffer.data + i);
        pos += 2;
    }

    push_block(name, ctx, data_size,
        json_object_get_array(json_object(block_json_value), "length-blocks"), IRREND);
    ctx->buffer.idx += data_size;

    return SUCCESS;
}

static endianess_t get_endianess(struct json_value_t *block_json_value)
{
    const char *value = json_object_get_string(json_object(block_json_value), "endianess");
    if (!value)
        return BIGEND;

    if (!strcmp(value, "little"))
        return LITEND;
    else if (!strcmp(value, "big"))
        return BIGEND;

    err("Erroneous endian specification: %s", value);
    return ERREND;
}

static int consume_qword(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    uint64_t qword;
    const char *value;
    endianess_t endian = get_endianess(block_json_value);
    if (endian == ERREND) {
        return FAILURE;
    }

    value = json_object_get_string(json_object(block_json_value), "value");
    if (!value) {
        err("Failed to retrieve dword value from block");
        return FAILURE;
    }

    qword = hexstr_to_qword(value, endian);
    if (qword == 0 && errno != 1) {
        err("JSON value cannot be represented as a qword: %s", value);
        return FAILURE;
    }

    if (realloc_data_buffer(ctx, sizeof(qword)))
        return FAILURE;

    memcpy(ctx->buffer.data + ctx->buffer.idx, &qword, sizeof(qword));
    push_block(name, ctx, sizeof(qword),
        json_object_get_array(json_object(block_json_value), "length-blocks"), endian);
    ctx->buffer.idx += sizeof(qword);

    return SUCCESS;
}

static int consume_dword(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    uint32_t dword;
    const char *value;
    endianess_t endian = get_endianess(block_json_value);
    if (endian == ERREND) {
        return FAILURE;
    }

    value = json_object_get_string(json_object(block_json_value), "value");
    if (!value) {
        err("Failed to retrieve dword value from block");
        return FAILURE;
    }

    dword = hexstr_to_dword(value, endian);
    if (dword == 0 && errno != 0) {
        err("JSON value cannot be represented as a dword: %s", value);
        return FAILURE;
    }

    if (realloc_data_buffer(ctx, sizeof(dword)))
        return FAILURE;

    memcpy(ctx->buffer.data + ctx->buffer.idx, &dword, sizeof(dword));
    push_block(name, ctx, sizeof(dword),
        json_object_get_array(json_object(block_json_value), "length-blocks"), endian);
    ctx->buffer.idx += sizeof(dword);

    return SUCCESS;
}

static int consume_word(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    uint16_t word;
    const char *value;
    endianess_t endian = get_endianess(block_json_value);
    if (endian == ERREND) {
        return FAILURE;
    }

    value = json_object_get_string(json_object(block_json_value), "value");
    if (!value) {
        err("Failed to retrieve dword value from block");
        return FAILURE;
    }

    word = hexstr_to_word(value, endian);
    if (word == 0 && errno != 0) {
        err("JSON value cannot be represented as a word: %s", value);
        return FAILURE;
    }

    if (realloc_data_buffer(ctx, sizeof(word)))
        return FAILURE;

    memcpy(ctx->buffer.data + ctx->buffer.idx, &word, sizeof(word));
    push_block(name, ctx, sizeof(word),
        json_object_get_array(json_object(block_json_value), "length-blocks"), endian);
    ctx->buffer.idx += sizeof(word);

    return SUCCESS;
}

static int consume_byte(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    uint8_t byte;
    const char *value;

    value = json_object_get_string(json_object(block_json_value), "value");
    if (!value) {
        err("Failed to retrieve byte value from block");
        return FAILURE;
    }

    byte = hexstr_to_byte(value);
    if (byte == 0 && errno != 0) {
        err("JSON value cannot be converted to a byte: %s", value);
        return FAILURE;
    }

    if (realloc_data_buffer(ctx, sizeof(byte)))
        return FAILURE;

    memcpy(ctx->buffer.data + ctx->buffer.idx, &byte, sizeof(byte));
    push_block(name, ctx, sizeof(byte),
        json_object_get_array(json_object(block_json_value), "length-blocks"), IRREND);
    ctx->buffer.idx += sizeof(byte);

    return SUCCESS;
}

static int consume_number(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    const char *type = json_object_get_string(json_object(block_json_value), "type");
    if (!type) {
        err("Failed to retrieve number type");
        return FAILURE;
    }

    // TODO make this a hashmap
    if (!strcmp(type, "qword"))
        return consume_qword(name, block_json_value, ctx);
    else if (!strcmp(type, "dword"))
        return consume_dword(name, block_json_value, ctx);
    else if (!strcmp(type, "word"))
        return consume_word(name, block_json_value, ctx);
    else if (!strcmp(type, "byte"))
        return consume_byte(name, block_json_value, ctx);

    err("Unsupported number type: %s\n", type);
    return FAILURE;
}

static int reserve_length(const char *name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    const char *type;
    endianess_t endian;
    size_t size;

    type = json_object_get_string(json_object(block_json_value), "type");
    if (!type) {
        err("Failed to parse type for length field");
        return FAILURE;
    }

    endian = get_endianess(block_json_value);

    // TODO make this a map
    if (!strcmp(type, "qword")) {
        size = sizeof(uint64_t);
    } else if (!strcmp(type, "dword")) {
        size = sizeof(uint32_t);
    } else if (!strcmp(type, "word")) {
        size = sizeof(uint16_t);
    } else if (!strcmp(type, "byte")) {
        size = sizeof(uint8_t);
    } else {
        err("Unsupported length type: %s", type);
        return FAILURE;
    }

    if (realloc_data_buffer(ctx, size))
        return FAILURE;

    memset(ctx->buffer.data + ctx->buffer.idx, 0, size);
    push_block(name, ctx, size,
        json_object_get_array(json_object(block_json_value), "length-blocks"), endian);
    ctx->buffer.idx += size;
    return SUCCESS;    
}

static int handle_block(const char * name, struct json_value_t *block_json_value, flyr_t *ctx)
{
    const char *class = json_object_get_string(json_object(block_json_value), "class");
    if (!class) {
        err("Failed to retrieve block class");
        return FAILURE;
    }

    if (!strcmp(class, "hex"))
        return consume_hexstr(name, block_json_value, ctx);
    else if (!strcmp(class, "number"))
        return consume_number(name, block_json_value, ctx);
    else if (!strcmp(class, "length"))
        return reserve_length(name, block_json_value, ctx);

    err("Unsupported block class: %s\n", class);
    return FAILURE;
}

int iterate_blocks(flyr_t *ctx)
{
    struct json_value_t *block_json_value = NULL;
    const char *name;
    for (ctx->blocks->idx = 0; ctx->blocks->idx < ctx->blocks->count; ctx->blocks->idx++) {
        name = json_object_get_name(json_object(ctx->blocks->json_value), ctx->blocks->idx);
        info("  -- %s", name);
        block_json_value = json_object_get_value_at(
                json_object(ctx->blocks->json_value), ctx->blocks->idx);
        if (!block_json_value) {
            err("Failed to retrieve next JSON block (idx: %lu", ctx->blocks->idx);
            return FAILURE;
        }

        if (handle_block(name, block_json_value, ctx) == FAILURE)
            return FAILURE;
    }

    if (fixup_length_blocks(ctx)) {
        err("Failed to fixup length blocks");
        return FAILURE;
    }

    return SUCCESS;
}
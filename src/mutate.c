#include "mutate.h"
#include "utils.h"
#include  <errno.h>
#include <limits.h>

#define BITS_IN_BYTE (8)
#define BITFLIP(ptr, pos) \
    *ptr ^= 1UL << pos;

static int32_t _hex_string_to_integer(const char *hexstr)
{
    long val;

    if (!hexstr)
        return -1;

    if (strlen(hexstr) > 2 && !strncmp(hexstr, "0x", 2))
        val = strtol(hexstr, NULL, 16); // hex
    else
        val = strtol(hexstr, NULL, 10); // decimal

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
        (errno != 0 && val == 0)) {
        return -1;
    }

    return (int32_t)val;
}

static int _bitflip_and_invoke_callback(uint32_t start, uint32_t stop,
    dud_t *ctx, callback_t callback)
{
    size_t i, j;
    char saved;
    for (i = start; i <= stop; i++) {
        saved = ctx->buffer.data[i];
        for (j = 0; j < BITS_IN_BYTE; j++) {
            ctx->buffer.data[i] = saved;
            BITFLIP(&ctx->buffer.data[i], j);
            if (callback(ctx) != SUCCESS) {
                ctx->buffer.data[i] = saved;
                return FAILURE;
            }
        }
    }

    return SUCCESS;
}

static int _handle_bitflip_mutation(struct json_value_t *action_json_value,
    dud_t *ctx, callback_t callback)
{
    (void)action_json_value;
    int32_t start_offset, stop_offset;

    start_offset = _hex_string_to_integer(
        json_object_get_string(json_object(action_json_value), "start")
    );

    if (start_offset < 0) {
        duderr("Erroneous mutation start offset");
        return FAILURE;
    }

    stop_offset = _hex_string_to_integer(
        json_object_get_string(json_object(action_json_value), "stop")
    );

    if (stop_offset < 0) {
        duderr("Erroneous mutation stop offset");
        return FAILURE;
    }

    if ((size_t)stop_offset > ctx->buffer.size) {
        duderr("Bitflip stop offset exceeds end of buffer");
        return FAILURE;
    }

    return _bitflip_and_invoke_callback((uint32_t)start_offset,
        (uint32_t)stop_offset, ctx, callback);
}

static int _handle_mutation(struct json_value_t *action_json_value, dud_t *ctx, callback_t callback)
{
    const char *action = json_object_get_string(json_object(action_json_value), "action");
    if (!action) {
        duderr("Failed to parse mutation action from JSON");
        return FAILURE;
    }

    if (strstr(action, "bitflip"))
        return _handle_bitflip_mutation(action_json_value, ctx, callback);

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

        _handle_mutation(action_json_value, ctx, callback);
    }

    return SUCCESS;
}
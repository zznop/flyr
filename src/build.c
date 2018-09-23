#include "build.h"
#include "utils.h"

static int _consume_hexstr(const char *hexstr, dud_t *ctx)
{
    size_t data_size = 0;
    const char *pos = NULL;
    size_t i = 0;
    uint8_t *tmp = NULL;

    if (hexstr[strspn(hexstr, "0123456789abcdefABCDEF")]) {
        duderr("Input data is not a valid hex string");
        return FAILURE;
    }

    data_size = strlen(hexstr) / 2;
    if (!ctx->buffer.data) {
        ctx->buffer.data = (uint8_t *)malloc(data_size);
        if (!ctx->buffer.data) {
            duderr("Out of memory");
            return FAILURE;
        }

        ctx->buffer.ptr = ctx->buffer.data;
        ctx->buffer.size = data_size;
    } else {
        tmp = realloc(ctx->buffer.data, ctx->buffer.size + data_size);
        if (!ctx->buffer.data) {
            duderr("Out of memory");
            return FAILURE;
        }

        ctx->buffer.data = tmp;
        ctx->buffer.size += data_size;
    }

    pos = hexstr;
    for (i = 0; i < data_size; i++, ctx->buffer.ptr++) {
        sscanf(pos, "%2hhx", ctx->buffer.ptr);
        pos += 2;
    }

    return SUCCESS;
}

static int _consume_data(struct json_value_t *action_json_value, dud_t *ctx)
{
    const char *type = NULL;
    const char *data_str = NULL;

    type = json_object_get_string(json_object(action_json_value), "type");
    if (!type) {
        duderr("Failed to read data type from action");
        return FAILURE;
    }

    data_str = json_object_get_string(json_object(action_json_value), "data");
    if (!data_str) {
        duderr("Failed to read data from action");
        return FAILURE;
    }

    if (strstr(type, "hex")) {
        return _consume_hexstr(data_str, ctx);
    } else {
        duderr("Unsupported data type: %s\n", type);
        return FAILURE;
    }

    return SUCCESS;
}

static int _handle_action(struct json_value_t *action_json_value, dud_t *ctx)
{
    const char *action = json_object_get_string(json_object(action_json_value), "action");
    if (!action) {
        duderr("Failed to retrieve action");
        return FAILURE;
    }

    if (strstr(action, "consume"))
        return _consume_data(action_json_value, ctx);

    return FAILURE;
}

int iterate_actions(dud_t *ctx)
{
    struct json_value_t *action_json_value = NULL;

    for (ctx->actions->idx = 0; ctx->actions->idx < ctx->actions->count; ctx->actions->idx++) {
        action_json_value = json_object_get_value_at(
                json_object(ctx->actions->json_value), ctx->actions->idx);
        if (!action_json_value) {
            duderr("Failed to retrieve next JSON action (idx: %lu", ctx->actions->idx);
            return FAILURE;
        }

        if (_handle_action(action_json_value, ctx) == FAILURE)
            return FAILURE;
    }

    return SUCCESS;
}
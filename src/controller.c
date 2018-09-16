/*
 * controller.c
 */

#include "utils.h"
#include "controller.h"

/**
 * Initializes the actions handler
 */
static struct actions_handler *_init_actions_handler(struct json_value_t *json_root)
{
    struct actions_handler *actions = (struct actions_handler *)malloc(sizeof(struct actions_handler));
    if (!actions) {
        duderr("Out of memory");
        return NULL;
    }

    actions->json_value = json_object_get_value(json_object(json_root), "actions");
    if (!actions->json_value) {
        duderr("Failed to retrieve \"actions\" JSON value");
        return NULL;
    }

    actions->count = json_object_get_count(json_object(actions->json_value));
    if (!actions->count) {
        duderr("Failed to retrieve number of actions \"num-actions\"");
        return NULL;
    }

    actions->idx = 0;

    return actions;
}

/**
 * Parse file output parameters and set _output_params to use them
 */
static struct output_handler *_set_output_params(struct json_value_t *json_output_value)
{
    struct output_handler *output = NULL;
    const char *directory_path = NULL;
    const char *name_suffix = NULL;
    struct output_params *fout_params = NULL;

    directory_path = json_object_get_string(
            json_object(json_output_value), "directory-path");
    if (!directory_path) {
        duderr("Export directory path not supplied: \"directory-path\"");
        goto fail;
    }

    name_suffix = json_object_get_string(
            json_object(json_output_value), "name-suffix");
    if (!name_suffix) {
        duderr("Name suffix for exported files not supplied: \"name-suffix\"");
        goto fail;
    }

    fout_params = (struct output_params *)malloc(sizeof(struct output_params));
    if (!fout_params) {
        duderr("Out of memory");
        goto fail;
    }

    fout_params->directory_path = directory_path;
    fout_params->name_suffix = name_suffix;

    output = (struct output_handler *)malloc(sizeof(struct output_handler));
    if (!output) {
        duderr("Out of memory");
        goto fail;
    }

    output->method = OUTPUT_FILEOUT;
    output->json_value = json_output_value;
    output->params = fout_params;

    return output;
fail:
    duderr("Out of memory");
    if (fout_params) {
        free(fout_params);
    }

    if (output) {
        free(output);
        output = NULL;
    }

    return NULL;
}

/**
 * Parse output parameters from JSON file and set parameters
 */
static struct output_handler *_init_output_handler(struct json_value_t *json_root)
{
    struct output_handler *output = NULL;
    struct json_value_t *json_output_value = NULL;
    const char *method = NULL;

    json_output_value = json_object_get_value(json_object(json_root), "output");
    if (!json_output_value) {
        duderr("failed to parse JSON output value");
        return NULL;
    }

    method = json_object_get_string(json_object(json_output_value), "method");
    if (!method) {
        duderr("output method was not specified");
        return NULL;
    }

    if (!strcmp(method, "file-out")) {
        output = _set_output_params(json_output_value);
    } else {
        duderr("unsupported export method: %s", method);
        return NULL;
    }

    return output;
}

/**
 * Convert hext string to byte array and append it to the buffer
 */
static int _consume_hexstr(const char *hexstr, dud_t *ctx)
{
    size_t data_size = 0;
    const char *pos = NULL;
    size_t i = 0;
    uint8_t *tmp = NULL;

    // Ensure it's a valid hex string
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

/**
 * Parse and input data by specified type
 */
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

/**
 * Parse and handle action by name
 */
static int _handle_action(struct json_value_t *action_json_value, dud_t *ctx)
{
    const char *action = json_object_get_string(json_object(action_json_value), "action");
    if (!action) {
        duderr("Failed to retrieve action");
        return FAILURE;
    }

    if (strstr(action, "consume")) {
        return _consume_data(action_json_value, ctx);
    }

    return SUCCESS;
}

/**
 * Iterate actions and construct template data buffer
 */
int dudley_iterate_actions(dud_t *ctx)
{
    struct json_value_t *action_json_value = NULL;

    for (ctx->actions->idx = 0; ctx->actions->idx < ctx->actions->count; ctx->actions->idx++) {
        action_json_value = json_object_get_value_at(
                json_object(ctx->actions->json_value), ctx->actions->idx);
        if (!action_json_value) {
            duderr("Failed to retrieve next JSON action (idx: %lu", ctx->actions->idx);
            return FAILURE;
        }

        if (!_handle_action(action_json_value, ctx)) {
            return FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * Cleanup dudley context
 */
void dudley_destroy(dud_t *ctx)
{
    if (ctx->actions) {
        free(ctx->actions);
    }

    if (ctx->output) {
        if (ctx->output->params) {
            free(ctx->output->params);
        }

        free(ctx->output);
    }

    if (ctx->json_root) {
        json_value_free(ctx->json_root);
    }

    if (ctx->buffer.data) {
        free(ctx->buffer.data);
    }

    free(ctx);
    ctx = NULL;
}

/**
 * Parse dudley JSON file and validate the schema
 */
dud_t *dudley_load_file(const char *filepath)
{
    const char *name = NULL;
    struct json_value_t *json_root = NULL;
    struct output_handler *output = NULL;
    struct actions_handler *actions = NULL;
    dud_t *ctx = NULL;
    struct json_value_t *schema = json_parse_string(
        "{"
            "\"name\":\"\","
            "\"output\": {},"
            "\"actions\": {}"
        "}"
    );

    json_root = json_parse_file(filepath);
    if (!json_root) {
        duderr("JSON formatted input is invalid");
        goto done;
    }

    if (json_validate(schema, json_root) != JSONSuccess) {
        duderr("Erroneous JSON schema");
        json_value_free(json_root);
        goto done;
    }

    name = json_object_get_string(json_object(json_root), "name");
    dudinfo("%s (%s) loaded successfully!", filepath, name);

    output = _init_output_handler(json_root);
    if (!output) {
        duderr("Failed to parse the output parameters");
        goto done;
    }

    actions = _init_actions_handler(json_root);
    if (!actions) {
        duderr("Failed to initialize the actions handler");
        goto done;
    }

    ctx = (dud_t *)malloc(sizeof(dud_t));
    if (!ctx) {
        duderr("Out of memory");
        goto done;
    }

    ctx->name = name;
    ctx->json_root = json_root;
    ctx->actions = actions;
    ctx->output = output;
    ctx->buffer.data = NULL;
    ctx->buffer.ptr = NULL;
    ctx->buffer.size = 0;

done:
    json_value_free(schema);
    return ctx;
}

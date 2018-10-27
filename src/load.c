/**
 * load.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "load.h"
#include "utils.h"
#include "mutate.h"
#include "parson/parson.h"
#include <sys/stat.h>

static mutations_t *init_mutations_ctx(struct json_value_t *json_root)
{
    mutations_t *mutations = (mutations_t *)malloc(sizeof(mutations_t));
    if (!mutations) {
        duderr("Out of memory");
        return NULL;
    }

    mutations->json_value = json_object_get_value(json_object(json_root), "mutations");
    if (!mutations->json_value) {
        duderr("Failed to retrieve \"mutations\" JSON value");
        return NULL;
    }

    mutations->count = json_object_get_count(json_object(mutations->json_value));
    if (!mutations->count) {
        duderr("Failed to retrieve \"mutations\" JSON value");
        return NULL;
    }

    mutations->idx = 0;
    return mutations;
}

static blocks_t *init_blocks_ctx(struct json_value_t *json_root)
{
    blocks_t *blocks = (blocks_t *)malloc(sizeof(blocks_t));
    if (!blocks) {
        duderr("Out of memory");
        return NULL;
    }

    blocks->json_value = json_object_get_value(json_object(json_root), "blocks");
    if (!blocks->json_value) {
        duderr("Failed to retrieve \"blocks\" JSON value");
        return NULL;
    }

    blocks->count = json_object_get_count(json_object(blocks->json_value));
    if (!blocks->count) {
        duderr("Failed to get blocks count");
        return NULL;
    }

    blocks->idx = 0;
    return blocks;
}

static output_t *set_output_params(struct json_value_t *json_output_value)
{
    output_t *output = NULL;
    const char *directory_path = NULL;
    const char *name_suffix = NULL;
    struct output_params *fout_params = NULL;
    struct stat st = {0};

    directory_path = json_object_get_string(
            json_object(json_output_value), "directory-path");
    if (!directory_path) {
        duderr("Export directory path not supplied: \"directory-path\"");
        goto fail;
    }

    if (stat(directory_path, &st) == -1) {
        duderr("Directory does not exist: %s", directory_path);
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

    output = (output_t *)malloc(sizeof(output_t));
    if (!output) {
        duderr("Out of memory");
        goto fail;
    }

    output->method = OUTPUT_FILEOUT;
    output->json_value = json_output_value;
    output->params = fout_params;

    return output;
fail:
    if (fout_params)
        free(fout_params);

    if (output)
        free(output);

    return NULL;
}

static output_t *init_output_ctx(struct json_value_t *json_root)
{
    output_t *output = NULL;
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
        output = set_output_params(json_output_value);
    } else {
        duderr("unsupported export method: %s", method);
        return NULL;
    }

    return output;
}

dud_t *load_file(const char *filepath)
{
    const char *name = NULL;
    struct json_value_t *json_root = NULL;
    output_t *output = NULL;
    mutations_t *mutations = NULL;
    blocks_t *blocks = NULL;
    dud_t *ctx = NULL;
    struct json_value_t *schema = json_parse_string(
        "{"
            "\"name\":\"\","
            "\"output\": {},"
            "\"blocks\": {},"
            "\"mutations\": {}"
        "}"
    );

    json_root = json_parse_file_with_comments(filepath);
    if (!json_root) {
        duderr("Failed to open dudley file");
        goto done;
    }

    if (json_validate(schema, json_root) != JSONSuccess) {
        duderr("Erroneous JSON schema");
        json_value_free(json_root);
        goto done;
    }

    name = json_object_get_string(json_object(json_root), "name");
    dudinfo("%s (%s) loaded successfully!", filepath, name);

    output = init_output_ctx(json_root);
    if (!output) {
        duderr("Failed to parse the output parameters");
        goto done;
    }

    blocks = init_blocks_ctx(json_root);
    if (!blocks) {
        duderr("Failed to initialize the blocks handler");
        goto done;
    }

    mutations = init_mutations_ctx(json_root);
    if (!mutations) {
        duderr("Failed to initialize the mutations handler");
        goto done;
    }

    ctx = (dud_t *)malloc(sizeof(dud_t));
    if (!ctx) {
        duderr("Out of memory");
        goto done;
    }

    ctx->name = name;
    ctx->json_root = json_root;
    ctx->blocks = blocks;
    ctx->blocks->list = NULL;
    ctx->mutations = mutations;
    ctx->output = output;
    ctx->buffer.data = NULL;
    ctx->buffer.idx = 0;
    ctx->buffer.size = 0;

done:
    json_value_free(schema);
    return ctx;
}

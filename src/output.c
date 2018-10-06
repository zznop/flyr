/**
 * output.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "output.h"
#include "utils.h"
#include "parse.h"

#define MAX_FILENAME_SIZE 128

static size_t count = 0;

static int output_data_to_file(dud_t *ctx)
{
    char filename[MAX_FILENAME_SIZE];
    FILE *file = NULL;
    int ret = FAILURE;

    snprintf(
        filename, MAX_FILENAME_SIZE, "%s/%08ld-%s",
        ctx->output->params->directory_path, count, ctx->output->params->name_suffix
    );

    file = fopen(filename, "wb");
    if (!file) {
        duderr("Failed to open file: %s", filename);
        goto out;
    }

    if (fwrite(ctx->buffer.data, sizeof(uint8_t), ctx->buffer.size, file) != ctx->buffer.size) {
        duderr("Failed to write to file");
        goto out;
    }

    ret = SUCCESS;
out:
    count++;

    if (file)
        fclose(file);

    return ret;
}

int output_mutated_data(dud_t *ctx)
{
    switch (ctx->output->method) {
    case OUTPUT_FILEOUT:
        return output_data_to_file(ctx);
    }

    return FAILURE;
}
#include "output.h"
#include "utils.h"
#include "parse.h"
#include <sys/time.h>

#define MAX_FILENAME_SIZE 128

static int _output_data_to_file(dud_t *ctx)
{
    char filename[MAX_FILENAME_SIZE];
    struct timeval currtime;
    FILE *file = NULL;
    int ret = FAILURE;

    gettimeofday(&currtime, NULL);
    snprintf(
        filename, MAX_FILENAME_SIZE, "%s/%ld_%ld_%s",
        ctx->output->params->directory_path, currtime.tv_sec,
        currtime.tv_usec, ctx->output->params->name_suffix
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
    if (file)
        fclose(file);

    return ret;
}

int output_mutated_data(dud_t *ctx)
{
    switch (ctx->output->method) {
    case OUTPUT_FILEOUT:
        return _output_data_to_file(ctx);
    }

    return FAILURE;
}
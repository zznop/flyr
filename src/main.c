/**
 * main.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "utils.h"
#include "flyr.h"
#include "build.h"
#include "mutate.h"
#include "output.h"
#include "load.h"
#include "unload.h"


static void print_help(void)
{
    printf(
        "./flyr [options] -f [file]\n"
        "-f FILE    Path to flyr JSON formatted fuzzing model\n"
    );
}

int main(int argc, char **argv)
{
    int opt = 0;
    char *filepath = NULL;
    flyr_t *ctx = NULL;
    int ret = FAILURE;

    if (argc < 2) {
        print_help();
        goto out;
    }

    while ((opt = getopt(argc, argv, "hf:")) != -1) {
        switch (opt) {
        case 'h':
            print_help();
            goto out;

        case 'f':
            filepath = optarg;
            break;

        default:
            return FAILURE;
        }
    }

    if (!filepath) {
        err("you must supply a filepath");
        goto out;
    }

    info("Loading flyr file and validating the JSON schema");
    ctx = load_file(filepath);
    if (!ctx)
        goto out;

    info("Applying build actions...");
    if (iterate_blocks(ctx) == FAILURE)
        goto out;

    info("Processing mutations...");
    if (iterate_mutations(ctx, output_mutated_data) == FAILURE)
        goto out;

    info("Done.");
    ret = SUCCESS;
out:
    if (ctx)
        destroy_context(ctx);

    return ret;
}

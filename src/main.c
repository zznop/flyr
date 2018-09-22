/*
 * main.c
 */

#include "utils.h"
#include "main.h"
#include "parse.h"
#include "build.h"
#include "mutate.h"

/**
 * Display command-line interface usage
 */
static void print_help(void)
{
    printf(
        "./dudley [options] -f [file]\n"
        "-f FILE    Path to dudley JSON formatted fuzzing model\n"
    );
}

/**
 * Parse arguments and execute a dudley file
 */
int main(int argc, char **argv)
{
    int opt = 0;
    char *filepath = NULL;
    dud_t *ctx = NULL;
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
        duderr("you must supply a filepath");
        goto out;
    }

    dudinfo("Loading dudley file and validating the JSON schema");
    ctx = load_file(filepath);
    if (!ctx) {
        goto out;
    }

    dudinfo("Applying build actions...");
    if (iterate_actions(ctx) == FAILURE) {
        goto out;
    }

    dudinfo("Processing mutations...");
    if (iterate_mutations(ctx) == FAILURE) {
        goto out;
    }

    ret = SUCCESS;
out:
    if (ctx) {
        destroy_context(ctx);
    }

    return ret;
}

/*
 * main.c
 */

#include "utils.h"
#include "main.h"

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
 * @param argc CLI arguments count
 * @param argv CLI arguments array
 * @return SUCCESS(0) or FAILURE(1)
 */
int main(int argc, char **argv)
{
    int opt = 0;
    char *filepath = NULL;

    if (argc < 2) {
        print_help();
        return SUCCESS;
    }

    while ((opt = getopt(argc, argv, "hf:")) != -1) {
        switch (opt) {
        case 'h':
            print_help();
            return SUCCESS;

        case 'f':
            filepath = optarg;
            break;

        default:
            return FAILURE;
        }
    }

    if (!filepath) {
        duderr("you must supply a filepath");
        return FAILURE;
    }

    printf("%s\n", filepath);
    return SUCCESS;
}

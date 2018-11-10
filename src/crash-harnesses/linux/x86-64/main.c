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

static void print_help(void)
{
    printf(
        "./flyr-crash [options]\n"
        "-e EXE    Path to executable to be fuzzed\n"
    );
}

int main(int argc, char **argv)
{
    int opt;
    int ret = FAILURE;

    if (argc < 2) {
        print_help();
        goto done;
    }

    while ((opt = getopt(argc, argv, "he:")) != -1) {
        switch (opt) {
        case 'h':
            print_help();
            return SUCCESS;
        case 'e':
            break;
        default:
            print_help();
            goto done;
        }
    }

done:
    return ret;
}
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
#include <spawn.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include "utils.h"

extern char **environ;

static void print_help(void)
{
    printf(
        "./flyr-crash [options]\n"
        "-e EXE    Path to executable to be fuzzed\n"
    );
}

static void spawn_process(char **argv)
{
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    execve(argv[0], argv, environ);

    // execve only returns on failure
    err("Failed to execute process");
    exit(1);
}

static int trace_process(pid_t pid)
{
    int status;
    waitpid(pid, &status, 0);
    ptrace(PTRACE_CONT, pid, 0, 0);
    waitpid(pid, &status, 0);
    ptrace(PTRACE_DETACH, pid, 0, 0);
    return 0;
}

int main(int argc, char **argv)
{
    int ret = FAILURE;
    int pid;

    if (argc < 2) {
        print_help();
        goto done;
    }

    pid = fork();
    if (!pid) {
        // This won't return
        spawn_process(&argv[1]);
    } else {
        ret = trace_process(pid);
    }
done:
    return ret;
}
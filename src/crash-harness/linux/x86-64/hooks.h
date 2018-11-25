#ifndef _HOOKS_H
#define _HOOKS_H

#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <elf.h>

typedef int (*callback_t)(pid_t pid, struct user_regs_struct *regs);

typedef enum {
    EXITED = 0,
    CRASHED,
    SYSCALL,
    UNKNOWN
} estat_t;

struct syshooks {
    uint64_t sysnum;
    callback_t callback;
};

int mmap_hook(pid_t pid, struct user_regs_struct *regs);

#endif

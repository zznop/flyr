#ifndef _HOOKS_H
#define _HOOKS_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <stdint.h>

typedef enum {
    EXITED = 0,
    CRASHED,
    SYSCALL,
    UNKNOWN
} estat_t;

struct syshooks {
    uint64_t sysnum;
    void *callback;
};

void *mmap_hook(void *addr, size_t length, int prot,
        int flags, int fd, off_t offset);

#endif

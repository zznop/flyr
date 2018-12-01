#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdint.h>
#include "libc_hooks.h"
#include "utils.h"

#define TAG_VAL 0x666C7972666C7972
#define __debug_break() __asm__ volatile("int $0x03");

#define FAILHARD(msg) \
    err(msg); \
    exit(1);

typedef uint64_t tag_t;
typedef void *(*malloc_t)(size_t size);

void *_libc = NULL;
int _is_initialized = 0;

void *malloc(size_t size)
{
    malloc_t malloc_real; 
    size_t new_size;
    uint8_t *ptr;

    __debug_break();
    sleep(10);
    malloc_real = (malloc_t)dlsym(_libc, "malloc");
    if (!malloc_real)
        FAILHARD("Failed to locate real malloc");

    new_size = size + sizeof(tag_t) * 2;
    ptr = malloc_real(new_size);
    if (!ptr)
        return NULL;

    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;
    return ptr + sizeof(tag_t);
}

static void __attribute__((constructor)) init(void)
{
    _libc = dlopen(LIBC_PATH, RTLD_LAZY);
    if (!_libc) {
        FAILHARD("Failed to dlopen() libc");
    } else {
        _is_initialized = 1;
    }
}

#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdint.h>
#include <pthread.h>

#define TAG_VAL 0x666C7972666C7972UL
#define LIBC_PATH "/lib/x86_64-linux-gnu/libc.so.6"
#define __debug_break() __asm__ volatile("int $0x03");

#define FAIL() \
    exit(1);

typedef uint64_t tag_t;
typedef void *(*malloc_t)(size_t size);
typedef void (*free_t)(void *ptr);

/**
 * Decrement the pointer before the tag so we can free at the correct pointer
 */
void free(void *ptr)
{
    free_t free_real;

    free_real = (free_t)dlsym(RTLD_NEXT, "free");
    if (!free_real)
        FAIL();

    free_real(ptr - sizeof(tag_t));
}

/**
 * Allocate buffer and prepend/append tags for OOB write detection
 */
void *malloc(size_t size)
{
    malloc_t malloc_real; 
    size_t new_size;
    uint8_t *ptr;

    malloc_real = (malloc_t)dlsym(RTLD_NEXT, "malloc");
    if (!malloc_real)
        FAIL();
  
    new_size = size + sizeof(tag_t) * 2;
    ptr = malloc_real(new_size);
    if (!ptr)
        return NULL;

    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;
    return ptr + sizeof(tag_t);
}

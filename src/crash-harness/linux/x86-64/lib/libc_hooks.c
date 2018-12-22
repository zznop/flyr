/**
 * libc_hooks.c
 *
 * Copyright (C) 2018 zznop, zznop0x90@gmail.com
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdarg.h>

typedef uint64_t tag_t;
typedef void *(*malloc_t)(size_t size);
typedef void *(*calloc_t)(size_t num, size_t size);
typedef void *(*realloc_t)(void *ptr, size_t new_size);
typedef void (*free_t)(void *ptr);
typedef void *(*memcpy_t)(void *destination, const void *source, size_t num);
typedef int (*printf_t)(const char *format, ...);
typedef int (*vsnprintf_t)(char *s, size_t n, const char *format, va_list arg);

#define TAG_VAL 0x666C7972666C7972UL
#define BUG() __asm__ volatile(".int 0xdeadc0de");
#define LOG_BUF_SIZE (256)

#define FAIL() \
    BUG()

struct alloc_info {
    void *base;
    size_t size;
    struct alloc_info* next;
};

/**
 * Globals
 */

static struct alloc_info *_tagged_allocs = NULL;
static malloc_t _malloc_real = NULL;
static calloc_t _calloc_real = NULL;
static free_t _free_real = NULL;
static uint8_t _dlsym_tmp_buffer[8192];

static void initialize()
{
    if (!_malloc_real) {
        _malloc_real = (malloc_t)dlsym(RTLD_NEXT, "malloc");
        if (!_malloc_real)
            FAIL();
    }

    if (!_calloc_real) {
        _calloc_real = (calloc_t)dlsym(RTLD_NEXT, "calloc");
        if (!_calloc_real)
            FAIL();
    }

    if (!_free_real) {
        _free_real = (free_t)dlsym(RTLD_NEXT, "free");
        if (!_free_real)
            FAIL();
    }
}

/**
 * Runs on load
 */
void __attribute__((constructor)) init(void)
{
    initialize();
}

/**
 * Debug print
 */
static void info(const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];
    vsnprintf_t real_vsnprintf = (vsnprintf_t)dlsym(RTLD_NEXT, "vsnprintf");
    printf_t real_printf = (printf_t)dlsym(RTLD_NEXT, "printf");

    va_start(ap, fmt);
    real_vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);
    real_printf("\033[22;34m%s\033[0m\n", buf);
}

/**
 * Lookup if an allocation is tagged, or not
 */
static int is_tagged_allocation(void *ptr)
{
    struct alloc_info *curr;
    curr = _tagged_allocs;
    while (curr != NULL) {
        if (curr->base == ptr)
            return 1;

        curr = curr->next;
    }

    return 0;
}

/**
 * Push a tagged allocation to the end of the linked list
 */
static void push_alloc(struct alloc_info *alloc)
{
    struct alloc_info *curr;

    if (!_tagged_allocs) {
        _tagged_allocs = alloc;
        return;
    }

    curr = _tagged_allocs;
    while (curr->next != NULL)
        curr = curr->next;

    curr->next = alloc;
}

/**
 * Remove a free'd allocation from the tagged allocation list
 */
static void pop_alloc(void *ptr)
{
    struct alloc_info *curr, *prev;

    curr = _tagged_allocs;
    while (curr != NULL) {
        if (curr->base != ptr)
            goto next;

        // First alloc in list?
        if (curr == _tagged_allocs) {

            // Increment _tagged_allocs base if next is populated,
            // otherwise set base to NULL
            if (curr->next  != NULL)
                _tagged_allocs = curr->next;
            else
                _tagged_allocs = NULL;

            _free_real(curr);
            break;
        }

        prev->next = curr->next;
        _free_real(curr);
        break;
next:
        prev = curr;
        curr = curr->next;
    }
}

/**
 * Check if any tags have been overwritten
 */
static void check_tagged_allocs(void)
{
    struct alloc_info *curr;

    curr = _tagged_allocs;
    while (curr != NULL) {
        if ((uint64_t)(*(uint64_t *)curr->base) != TAG_VAL ||
            (uint64_t)(*(uint64_t *)(curr->base + curr->size - sizeof(TAG_VAL)) != TAG_VAL)) {
            info("\n############################################################");
            info("# OOB write detected!!!");
            info("#  -- alloc base: %llx", curr->base - sizeof(TAG_VAL));
            info("#  -- alloc size: %u", curr->size - sizeof(TAG_VAL) * 2);
            info("############################################################\n");
            BUG();
        }

        curr = curr->next;
    }
}

/**
 *  libc:memcpy hook for OOB write detection
 */
void *memcpy (void *destination, const void *source, size_t num)
{
    void *ret;
    memcpy_t memcpy_real;

    initialize();
    check_tagged_allocs();

    memcpy_real = (memcpy_t)dlsym(RTLD_NEXT, "memcpy");
    if (!memcpy_real)
        FAIL();

    ret = memcpy_real(destination, source, num);

    return ret;
}

/**
 * libc:free hook for OOB write detection
 */
void free(void *ptr)
{
    initialize();
    check_tagged_allocs();

    if (!_free_real)
        return;

    if (is_tagged_allocation(ptr - sizeof(tag_t))) {
        ptr = ptr - sizeof(tag_t);
        _free_real(ptr);
        pop_alloc(ptr);
    } else {
        _free_real(ptr);
    }
}

/**
 * libc:calloc hook - tag allocations
 */
void *calloc(size_t num, size_t size)
{
    size_t new_size, i;
    uint8_t *ptr;
    struct alloc_info *alloc;

    if (!_calloc_real) {
        // Ghetto calloc ¯\_(ツ)_/¯
        for (i = 0 ;i < sizeof(_dlsym_tmp_buffer); i++)
            _dlsym_tmp_buffer[i] = 0;

        return _dlsym_tmp_buffer;
    }

    initialize();
    check_tagged_allocs();

    new_size = num * size + sizeof(tag_t) * 2;
    ptr = _calloc_real(new_size, 1);
    if (!ptr)
        return NULL;

    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;

    alloc = _malloc_real(sizeof(*alloc));
    if (!alloc)
        FAIL();

    alloc->base = ptr;
    alloc->size = new_size;
    alloc->next = NULL;
    push_alloc(alloc);

    return ptr + sizeof(tag_t);
}

/**
 * lic:realloc hook
 */
void *realloc(void *ptr, size_t new_size)
{
    struct alloc_info *alloc;
    realloc_t realloc_real;

    initialize();
    check_tagged_allocs();

    realloc_real = (realloc_t)dlsym(RTLD_NEXT, "realloc");
    if (!realloc_real)
        FAIL();

    if (is_tagged_allocation(ptr - sizeof(tag_t))) {
        ptr = ptr - sizeof(tag_t);
        pop_alloc(ptr);
    }

    new_size = new_size + sizeof(tag_t) * 2;
    ptr = realloc_real(ptr, new_size);
    if (!ptr)
        return ptr;

    // Apply the tags
    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;

    // Cache the allocation info
    alloc = _malloc_real(sizeof(*alloc));
    if (!alloc)
        FAIL();

    alloc->base = ptr;
    alloc->size = new_size;
    alloc->next = NULL;
    push_alloc(alloc);

    return ptr + sizeof(tag_t);
}

/**
 * lic:malloc hook - tag allocations
 */
void *malloc(size_t size)
{
    size_t new_size;
    uint8_t *ptr;
    struct alloc_info *alloc;

    initialize();
    check_tagged_allocs();

    new_size = size + sizeof(tag_t) * 2;
    ptr = _malloc_real(new_size);
    if (!ptr)
        return NULL;

    // Apply the tags
    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;

    // Cache the allocation info
    alloc = _malloc_real(sizeof(*alloc));
    if (!alloc)
        FAIL();

    alloc->base = ptr;
    alloc->size = new_size;
    alloc->next = NULL;
    push_alloc(alloc);

    // Return as if base is after the tag
    return ptr + sizeof(tag_t);
}

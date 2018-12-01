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

struct alloc_info {
    void *base;
    size_t size;
    struct alloc_info* next;
};

struct alloc_info *_tagged_allocs;

/**
 * Lookup if an allocation is tagged, or not
 */
int is_tagged_allocation(void *ptr)
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
void push_alloc(struct alloc_info *alloc)
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

void pop_alloc(void *ptr, free_t free_real)
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

            free_real(curr);
            break;
        }

        prev->next = curr->next;
        free_real(curr);
        break;
next:
        prev = curr;
        curr = curr->next;
    }
}

/**
 * Decrement the pointer before the tag so we can free at the correct pointer
 */
void free(void *ptr)
{
    free_t free_real = (free_t)dlsym(RTLD_NEXT, "free");
    if (!free_real)
        FAIL();

    if (is_tagged_allocation(ptr - sizeof(tag_t))) {
        free_real(ptr - sizeof(tag_t));
    } else {
        free_real(ptr);
    }
}

/**
 * Allocate buffer and prepend/append tags for OOB write detection
 */
void *malloc(size_t size)
{
    malloc_t malloc_real; 
    size_t new_size;
    uint8_t *ptr;
    struct alloc_info *alloc;

    malloc_real = (malloc_t)dlsym(RTLD_NEXT, "malloc");
    if (!malloc_real)
        FAIL();
  
    new_size = size + sizeof(tag_t) * 2;
    ptr = malloc_real(new_size);
    if (!ptr)
        return NULL;

    // Apply the tags
    *(uint64_t *)ptr = TAG_VAL;
    *(uint64_t *)(ptr + new_size - sizeof(tag_t)) = TAG_VAL;

    // Cache the allocation info
    alloc = malloc_real(sizeof(*alloc));
    if (!alloc)
        FAIL();

    alloc->base = ptr;
    alloc->size = new_size;
    alloc->next = NULL;
    push_alloc(alloc);

    // Return as if base is after the tag
    return ptr + sizeof(tag_t);
}

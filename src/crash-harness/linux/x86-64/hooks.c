#include <sys/syscall.h>
#include "hooks.h"
#include "utils.h"

void *mmap_hook(void *addr, size_t length, int prot,
        int flags, int fd, off_t offset) {
   info("mmap hook hit!"); 
   return (void *)syscall(__NR_mmap, addr, length, prot, flags, fd, offset); 
}

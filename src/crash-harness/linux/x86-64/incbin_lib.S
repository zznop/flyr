.globl _hook_library
.globl _hook_library_size

_hook_library:
#ifdef DEBUG
    .incbin "./build/debug/crash/libflyr-hook.so"
#else
    .incbin "./build/release/crash/libflyr-hook.so"
#endif

_hook_library_size:
   .int _hook_library_size - _hook_library

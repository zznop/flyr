.globl _hook_library
.globl _hook_library_size

_hook_library:
    .incbin "./build/debug/hook/libflyr-hook.so"

_hook_library_size:
   .int _hook_library_size - _hook_library

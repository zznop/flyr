#include "hooks.h"
#include "utils.h"

int mmap_hook(pid_t pid, struct user_regs_struct *regs)
{
    (void)regs;
    (void)pid;
    info("mmap callback hit");

    if (!(regs->rdx & PROT_EXEC))
        return 0;

    regs->rdx = PROT_READ|PROT_WRITE|PROT_EXEC;
    ptrace(PTRACE_SETREGS, pid, 0, regs);
    return 0;
}

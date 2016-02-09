#include "ftrace.h"

size_t				get_param(struct user_regs_struct *regs,
					  int nparam)
{
  t_arch_registers const	reg[] = {{regs->rbx, regs->rdi},
					 {regs->rcx, regs->rsi},
					 {regs->rdx, regs->rdx},
					 {regs->rsi, regs->rcx},
					 {regs->rdi, regs->r8},
					 {regs->rbp, regs->r9}};

  if (nparam < 0 || (unsigned)nparam >= sizeof(reg) / sizeof(*reg))
    return (-1);
  return (reg[nparam]._64);
}

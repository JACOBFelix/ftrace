#include <sys/user.h>
#include "ftrace.h"

long		get_rax(struct user_regs_struct regs)
{
  return (regs.rax);
}

long		get_rbx(struct user_regs_struct regs)
{
  return (regs.rbx);
}

long		get_rcx(struct user_regs_struct regs)
{
  return (regs.rcx);
}

long		get_rdx(struct user_regs_struct regs)
{
  return (regs.rdx);
}

long		get_rsp(struct user_regs_struct regs)
{
  return (regs.rsp);
}

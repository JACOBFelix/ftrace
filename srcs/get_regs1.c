#include <sys/user.h>
#include "ftrace.h"

long		get_rbp(struct user_regs_struct regs)
{
  return (regs.rbp);
}

long		get_rsi(struct user_regs_struct regs)
{
  return (regs.rsi);
}

long		get_rdi(struct user_regs_struct regs)
{
  return (regs.rdi);
}

long		get_r8(struct user_regs_struct regs)
{
  return (regs.r8);
}

long		get_r9(struct user_regs_struct regs)
{
  return (regs.r9);
}

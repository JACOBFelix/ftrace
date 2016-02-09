#include <sys/user.h>
#include "ftrace.h"

long		get_r10(struct user_regs_struct regs)
{
  return (regs.r10);
}

long		get_r11(struct user_regs_struct regs)
{
  return (regs.r11);
}

long		get_r12(struct user_regs_struct regs)
{
  return (regs.r12);
}

long		get_r13(struct user_regs_struct regs)
{
  return (regs.r13);
}

long		get_r14(struct user_regs_struct regs)
{
  return (regs.r14);
}

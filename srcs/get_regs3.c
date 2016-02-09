#include <sys/user.h>
#include "ftrace.h"

long		get_r15(struct user_regs_struct regs)
{
  return (regs.r15);
}

long		get_rip(struct user_regs_struct regs)
{
  return (regs.rip);
}

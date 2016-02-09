#include <unistd.h>
#include "ftrace.h"

static int	print_mode(int mode)
{
  int		nb;
  int		wt;

  wt = 0;
  if (mode == F_OK)
    wt += printf("F_OK");
  else
  {
    nb = 0;
    if (mode & R_OK && ++nb)
      wt += printf("R_OK");
    if (mode & W_OK && ++nb)
      wt += printf("W_OK");
    if (mode & X_OK && ++nb)
      wt += printf("X_OK");
  }
  return (wt);
}

void		print_access(pid_t pid,
			     struct user_regs_struct *regs,
			     size_t return_value)
{
  int		nb;

  nb = 0;
  nb += printf("access(");
  nb += print_char_ptr(pid, get_param(regs, 0));
  nb += printf(", ");
  nb += print_mode(get_param(regs, 1));
  nb += printf(")");
  print_sspace(nb);
  printf("= ");
  print_int(pid, return_value);
  printf("\n");
}

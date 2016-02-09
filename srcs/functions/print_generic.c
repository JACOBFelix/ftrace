#include "ftrace.h"
#include "types.h"
#include "nums_syscalls.h"

t_func		g_functions_print[] =
{
  { INT, print_int },
  { SIZE_T, print_size_t },
  { OFF_T, print_off_t },
  { VOID_P, print_void_ptr },
  { CONST_VOID_P, print_void_ptr },
  { PVOID, print_void_ptr },
  { PCHAR, print_char_ptr },
  { CHAR_P, print_char_ptr },
  { CONST_CHAR_P, print_char_ptr },
  { CHAR_P_CONST, print_char_ptr },
  { UNSIGNED_CHAR_P, print_char_ptr },
  { -1, (void*)0 }
};

static int	print_type(pid_t pid, unsigned num, char nparam, size_t value)
{
  int		*type;
  size_t	i;
  int		nb;

  nb = 0;
  type = (int *)&g_syscalls[num].param1;
  if (nparam > 0)
    nb += printf(", ");
  if (nparam < 6)
  {
    i = 0;
    type += nparam;
    while (g_functions_print[i].type != -1
	   && g_functions_print[i].type != type[0])
      i += 1;
    if (g_functions_print[i].type != -1)
      nb += g_functions_print[i].fct(pid, value);
    else
      nb += printf("0x%lx", (size_t)value);
  }
  return (nb);
}

void		print_sspace(int nb)
{
  while (nb < 40)
  {
    printf(" ");
    nb += 1;
  }
}

void		print_generic(pid_t pid,
			      struct user_regs_struct *regs,
			      size_t return_value,
			      int num)
{
  int		sys;
  t_syscalls	*ptr;
  int		nb;

  nb = 0;
  ptr = (t_syscalls *)g_syscalls;
  sys = is_syscall_defined(num);
  nb += printf("%s(", ptr[sys].name);
  if (ptr[sys].nparams >= 1)
    nb += print_type(pid, num, 0, get_param(regs, 0));
  if (ptr[sys].nparams >= 2)
    nb += print_type(pid, num, 1, get_param(regs, 1));
  if (ptr[sys].nparams >= 3)
    nb += print_type(pid, num, 2, get_param(regs, 2));
  if (ptr[sys].nparams >= 4)
    nb += print_type(pid, num, 3, get_param(regs, 3));
  if (ptr[sys].nparams >= 5)
    nb += print_type(pid, num, 4, get_param(regs, 4));
  if (ptr[sys].nparams >= 6)
    nb += print_type(pid, num, 5, get_param(regs, 5));
  nb += printf(")");
  print_sspace(nb);
  nb += printf((return_value) ? ("= 0x%lx\n") : ("= %lx\n"), return_value);
}

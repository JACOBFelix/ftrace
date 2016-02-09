#include <sys/ptrace.h>
#include "ftrace.h"

static int	print_word(long word)
{
  char		*s;
  size_t	i;
  int		nb;

  i = 0;
  nb = 0;
  s = (char *)&word;
  while (i < 2)
  {
    if (s[i] >= 0x20 && s[i] <= 0x7E)
      nb += printf("%c", s[i]);
    else
      nb += printf("\\%o", (unsigned int)s[i]);
    i += 1;
  }
  return (nb);
}

static int	print_buf(pid_t pid, char *ptr, size_t len)
{
  size_t	i;
  long		text;
  int		nb;

  i = 0;
  nb = 0;
  nb += printf("\"");
  while (i < 16 && i < len)
  {
    text = ptrace(PTRACE_PEEKTEXT, pid, ptr, (void *)0);
    if (text >= 0)
    {
      nb += print_word(text);
    }
    i += 1;
    ptr += 2;
  }
  nb += printf("\"");
  if (i == 16)
  {
    nb += printf("...");
  }
  return (nb);
}

void		print_read(pid_t pid,
			   struct user_regs_struct *regs,
			   size_t return_value)
{
  int		nb;

  nb = 0;
  nb += printf("read(");
  nb += print_int(pid, get_param(regs, 0));
  nb += printf(", ");
  nb += print_buf(pid, (char *)get_param(regs, 1), get_param(regs, 2));
  nb += printf(", ");
  nb += print_size_t(pid, get_param(regs, 2));
  nb += printf(")");
  print_sspace(nb);
  printf("= ");
  print_ssize_t(pid, return_value);
  printf("\n");
}

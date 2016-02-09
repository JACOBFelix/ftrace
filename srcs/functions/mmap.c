#include <sys/ptrace.h>
#include <sys/mman.h>
#include "ftrace.h"

static void	print_proto(int proto)
{
  int		nb;

  if (!proto)
    printf("PROT_NONE");
  else
  {
    nb = 0;
    if (proto & PROT_READ && ++nb)
      printf("PROT_READ");
    if (proto & PROT_WRITE && ++nb)
      printf((nb) ? ("|PROT_WRITE") : ("PROT_WRITE"));
    if (proto & PROT_EXEC && ++nb)
      printf((nb) ? ("|PROT_EXEC") : ("PROT_EXEC"));
  }
}

static void	print_flags(int flags)
{
  printf((flags & MAP_PRIVATE) ? ("MAP_PRIVATE") : ("MAP_SHARED"));
  if (flags & MAP_FIXED)
    printf("|MAP_FIXED");
  if (flags & MAP_32BIT)
    printf("|MAP_32BIT");
  if (flags & MAP_ANONYMOUS)
    printf("|MAP_ANONYMOUS");
  if (flags & MAP_DENYWRITE)
    printf("|MAP_DENYWRITE");
  if (flags & MAP_GROWSDOWN)
    printf("|MAP_GROWSDOWN");
  if (flags & MAP_HUGETLB)
    printf("|MAP_HUGETLB");
  if (flags & MAP_LOCKED)
    printf("|MAP_LOCKED");
  if (flags & MAP_NONBLOCK)
    printf("|MAP_NONBLOCK");
  if (flags & MAP_NORESERVE)
    printf("|MAP_NORESERVE");
  if (flags & MAP_POPULATE)
    printf("|MAP_POPULATE");
  if (flags & MAP_STACK)
    printf("|MAP_STACK");
}

void		print_mmap(pid_t pid,
			   struct user_regs_struct *regs,
			   size_t return_value)
{
  printf("mmap(");
  print_void_ptr(pid, get_param(regs, 0));
  printf(", ");
  print_size_t(pid, get_param(regs, 1));
  printf(", ");
  print_proto(get_param(regs, 2));
  printf(", ");
  print_flags(get_param(regs, 3));
  printf(", ");
  print_int(pid, get_param(regs, 4));
  printf(", ");
  print_off_t(pid, get_param(regs, 5));
  printf(") = ");
  print_void_ptr(pid, return_value);
  printf("\n");
}

void		print_mmap2(pid_t pid,
			    struct user_regs_struct *regs,
			    size_t return_value)
{
  printf("mmap2(");
  if (!get_param(regs, 0))
    printf("NULL");
  else
    print_void_ptr(pid, get_param(regs, 0));
  printf(", ");
  print_size_t(pid, get_param(regs, 1));
  printf(", ");
  print_proto(get_param(regs, 2));
  printf(", ");
  print_flags(get_param(regs, 3));
  printf(", ");
  print_int(pid, get_param(regs, 4));
  printf(", ");
  print_off_t(pid, get_param(regs, 5) << 12);
  printf(") = ");
  print_void_ptr(pid, return_value);
  printf("\n");
}

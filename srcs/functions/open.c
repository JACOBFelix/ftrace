#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ftrace.h"

static int	print_flags2(int flags)
{
  int		nb;

  nb = 0;
  if (flags & O_DSYNC)
    nb += printf("|O_DSYNC");
  if (flags & O_EXCL)
    nb += printf("|O_EXCL");
  if (flags & O_NOCTTY)
    nb += printf("|O_NOCTTY");
  if (flags & O_NOFOLLOW)
    nb += printf("|O_NOFOLLOW");
  if (flags & O_NONBLOCK)
    nb += printf("|O_NONBLOCK");
  if (flags & O_SYNC)
    nb += printf("|O_SYNC");
  if (flags & O_TRUNC)
    nb += printf("|O_TRUNC");
  return (nb);
}

static int	print_flags(int flags)
{
  int		nb;

  nb = 0;
  if ((flags & O_RDONLY) == 0)
    nb += printf("O_RDONLY");
  else if ((flags & O_WRONLY) == 1)
    nb += printf("O_WRONLY");
  else if ((flags & O_RDWR) == 2)
    nb += printf("O_RDWR");
  if (flags & O_APPEND)
    nb += printf("|O_APPEND");
  if (flags & O_ASYNC)
    nb += printf("|O_ASYNC");
  if (flags & O_CLOEXEC)
    nb += printf("|O_CLOEXEC");
  if (flags & O_CREAT)
    nb += printf("|O_CREAT");
  if (flags & O_DIRECTORY)
    nb += printf("|O_DIRECTORY");
  nb += print_flags2(flags);
  return (nb);
}

static int	print_mode(int mode)
{
  int		n;

  n = 0;
  n = (mode & S_IRWXU) ? (700) : (n);
  n = (mode & S_IRUSR) ? (n + 400) : (n);
  n = (mode & S_IWUSR) ? (n + 200) : (n);
  n = (mode & S_IXUSR) ? (n + 100) : (n);
  n = (mode & S_IRWXG) ? (70) : (n);
  n = (mode & S_IRGRP) ? (n + 40) : (n);
  n = (mode & S_IWGRP) ? (n + 20) : (n);
  n = (mode & S_IXGRP) ? (n + 10) : (n);
  n = (mode & S_IRWXO) ? (7) : (n);
  n = (mode & S_IROTH) ? (n + 4) : (n);
  n = (mode & S_IWOTH) ? (n + 2) : (n);
  n = (mode & S_IXOTH) ? (n + 1) : (n);
  return (printf("%d", n));
}

void		print_open(pid_t pid,
			     struct user_regs_struct *regs,
			     size_t return_value)
{
  int		nb;

  nb = 0;
  nb += printf("open(");
  nb += print_char_ptr(pid, get_param(regs, 0));
  nb += printf(", ");
  nb += print_flags(get_param(regs, 1));
  if (get_param(regs, 1) & O_CREAT)
  {
    nb += printf(", ");
    nb += print_mode(get_param(regs, 3));
  }
  nb += printf(")");
  print_sspace(nb);
  printf("= ");
  print_int(pid, return_value);
  printf("\n");
}

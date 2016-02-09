#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <stdio.h>
#define PID
#include "ftrace.h"

void	        signal_on_exec(int a UNUSED)
{
  int		fd;

  fd = stock_fd(0);
  if (fd)
  {
    dprintf(fd, " } ");
    close(fd);
  }
  if (kill(g_pid, SIGKILL) == -1)
    printf("Can not kill process : %u\n", g_pid);
  exit(FAILURE);
}

void		signal_on_p(int a UNUSED)
{
  int		fd;

  fd = stock_fd(0);
  if (fd)
  {
    dprintf(fd, " } ");
    close(fd);
  }
  if (ptrace(PTRACE_DETACH, g_pid, NULL, NULL) == -1)
    {
      perror("ptrace");
      printf("Ptrace_detach failed.\n");
    }
  exit(FAILURE);
}

void		catch_signal(void(*fct)(int))
{
  signal(SIGINT, fct);
  signal(SIGTERM, fct);
  signal(SIGQUIT, fct);
}
#undef PID

#define PID
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "ftrace.h"

int		stop_son()
{
  if (kill(g_pid, SIGKILL) == -1)
    {
      perror(NULL);
      dprintf(2, "%s%u.\n", ERR_KILL_SON, g_pid);
    }
  return (FAILURE);
}

int		detach_process()
{
  return (FAILURE);
}
#undef PID

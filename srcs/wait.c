#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "ftrace.h"

int		my_wait(pid_t pid)
{
  int		status;

  if (wait4(pid, &status, WUNTRACED, NULL) <= -1)
    return (FAILURE);
  if (WIFEXITED(status))
    return (FAILURE);
  return (status);
}

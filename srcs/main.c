#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "ftrace.h"

static int	current(char const *const ag[],
			char const *const env[])
{
  char		*pwd;
  char		*tmp;
  int		a;

  a = FAILURE;
  if ((pwd = getenv("PWD")) == NULL)
    return (dprintf(2, "%s", GETENVPWD_ERR)
	    - sizeof(GETENVPWD_ERR) + FAILURE);
  if (asprintf(&tmp, "%s/%s", pwd, ag[0]) == -1)
    return (dprintf(2, "%s", MEMORY_ERR)
	    - sizeof(MEMORY_ERR) + FAILURE);
  if (access(tmp, X_OK) == 0)
    a = exec(tmp, ag, env);
  else
    return (dprintf(2, "%s%s\n", ACCESS_EXEC_ERR, tmp)
	    - sizeof(ACCESS_EXEC_ERR) - strlen(tmp) - 1+ FAILURE);
  free(tmp);
  return (a);
}

static int	path(char const *const ag[],
		     char const *const envp[])
{
  char		*path;
  char		*tmp;
  char		*env;
  int		a;

  if ((env = getenv("PATH")) == NULL)
    return (dprintf(2, "%s", GETENVPATH_ERR) - sizeof(GETENVPWD_ERR)
	    + FAILURE);
  while ((tmp = strtok(env, ":")) != NULL)
    {
      if (asprintf(&path, "%s/%s", tmp, ag[0]) == -1)
	return (dprintf(2, "%s", MEMORY_ERR) - sizeof(MEMORY_ERR)
		+ FAILURE);
      if (access(path, X_OK) == 0)
	{
	  a = exec(path, ag, envp);
	  free(path);
	  return (a);
	}
      free(path);
      env = NULL;
    }
  return (dprintf(2, "%s%s\n", INV_ARG, ag[0]) - sizeof(INV_ARG)
	  - strlen(ag[0]) + FAILURE);
}

static int	trace_exec(char const *const ag[],
			   char const *const env[])
{
  if (ag[0][0] == '.')
    return (current(ag, env));
  else if (ag[0][0] != '/')
    return (path(ag, env));
  else if (access(ag[0], X_OK) == 0)
    return (exec(ag[0], ag, env));
  return (dprintf(2, "%s%s\n", INV_ARG, ag[0]) - sizeof(INV_ARG)
	  - strlen(ag[0]) + FAILURE);
}

int	main(int ac UNUSED,
	     char const *const ag[],
	     char const *const env[])
{
  if (ac == 1)
    return (dprintf(2, "%s", NEED_ARG) - sizeof(NEED_ARG) + FAILURE);
  if (strcmp(ag[1], "-p") == 0)
    return (trace_pid(ag[2]));
  return (trace_exec(ag + 1, env));
}
#undef _GNU_SOURCE

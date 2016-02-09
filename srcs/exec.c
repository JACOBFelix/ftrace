#define PID
#define ELF
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "ftrace.h"

static int	getinstruction(struct user_regs_struct reg,
			       long ret,
			       t_symbol *symbols,
			       size_t ret_syscall)
{
  size_t	addr;
  int		is_a_call;

  if ((unsigned short)ret == 0x050F)
  {
    printf("@");
    disp_syscall(g_pid, &reg, reg.rax, ret_syscall);
    return (SUCCESS);
  }
  else if (instruction_is_ret(ret) != FAILURE)
  {
    printf(">>> return (0x%lX)\n", get_rax(reg));
    return (SUCCESS);
  }
  else if (instruction_is_call(ret) != FAILURE)
  {
    is_a_call = 0;
    addr = print_call(ret, reg, &is_a_call);
    resolve_symbol(addr, symbols, is_a_call);
    return (SUCCESS);
  }
  return (FAILURE);
}

int		stock_fd(int fd)
{
  static int	save = 0;

  if (!save)
  {
    save = fd;
  }
  return (save);
}

int		open_dotfile()
{
  int		fd;

  if ((fd = open("graph.dot", O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
    return (-1);
  stock_fd(fd);
  dprintf(fd, "digraph ftrace {\n");
  dprintf(fd, "bgcolor=azure;\n \
node [shape=box, color=lightblue2, style=filled];\n \
edge [arrowsize=1, color=red];\n");
  return (fd);
}

void		delete_quote(char *s)
{
  size_t	i;

  for (i = 0; s[i]; i += 1)
  {
    if (s[i] == '"')
      s[i] = '\'';
  }
}

void				format_dotfile(int fd,
					       char *buffer,
					       int first,
					       size_t len)
{
  static t_list_functions	*list_func = 0;
  static int			ret = 0;
  char				*func;

  buffer[len - 1] = 0;
  delete_quote(buffer);
  if (buffer[0] != '>')
  {
    if (ret)
    {
      func = get_function(list_func);
      dprintf(fd, "\n\"%s\" ", (func) ? (func) : ("U n K"));
      ret = 0;
    }
    if (first)
      write(fd, " -> ", 4);
    if (buffer[0] != '@')
      push_function(&list_func, buffer);
    else
      ret = 1;
    dprintf(fd, "\"%s\"", buffer);
    if (buffer[0] == '@')
      dprintf(fd, "\n\"%s\" [color=grey]", buffer);
  }
  else
  {
    pop_function(&list_func);
    ret = 1;
  }
}

int		print_instruction(struct user_regs_struct reg,
			       long ret,
			       t_symbol *symbols __attribute__ ((unused)),
			       size_t ret_syscall)
{
  char		buffer[2048];
  static int	first = 0;
  static int	fd = 0;

  if (!(((unsigned short)ret == 0x050F) || (instruction_is_ret(ret) != FAILURE)
	|| (instruction_is_call(ret) != FAILURE)))
    return (fd);
  memset(buffer, 0, sizeof(buffer));
  if (!first)
  {
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
    fd = open_dotfile();
  }
  getinstruction(reg, ret, symbols, ret_syscall);
  if (buffer[0])
    format_dotfile(fd, buffer, first, strlen(buffer));
  fflush(stdout);
  if (buffer[0])
    write(1, "\n", 1);
  first = 1;
  return (fd);
}

/*
**	Should free data in infos
*/
static int			trace_son(int (*fct)())
{
  struct user_regs_struct	reg;
  struct user_regs_struct	regs_return;
  long				ret;
  t_infos			infos;
  t_symbol			*symbols;
  int				status;
  static int			fd = 0;

  if (load_infos(&infos, fct) == FAILURE)
    return (FAILURE);
  symbols = get_symbols();
  waitpid(g_pid, &status, 0);
  while (WIFSTOPPED(status) && (WSTOPSIG(status) == SIGTRAP
				|| WSTOPSIG(status) == SIGSTOP))
  {
    ptrace(PTRACE_GETREGS, g_pid, NULL, &reg);
    ret = ptrace(PTRACE_PEEKTEXT, g_pid, reg.rip, 0);
    ptrace(PTRACE_SINGLESTEP, g_pid, NULL, NULL);
    waitpid(g_pid, &status, 0);
    ptrace(PTRACE_GETREGS, g_pid, 0, &regs_return);
    fd = print_instruction(reg, ret, symbols, regs_return.rax);
  }
  write(fd, "}\n", 2);
  close(fd);
  return (SUCCESS);
}

int		exec(char const *const exec,
		     char const *const ag[],
		     char const *const env[])
{
  if ((g_pid = fork()) == -1)
    return (dprintf(2, "%s", FORK_ERR) - sizeof(FORK_ERR) + FAILURE);
  else if (g_pid == 0)
    {
      if (ptrace(PTRACE_TRACEME, NULL, NULL) == -1)
	{
	  perror(NULL);
	  exit(dprintf(2, "%s", ERR_PTRACE_TRACEME) - sizeof(ERR_PTRACE_TRACEME)
	       + FAILURE);
	}
      if (execve((const char *)exec, (char *const *)ag, (char *const *)env)
	  == -1)
	exit(dprintf(2, "%s", EXECVE_ERR) - sizeof(EXECVE_ERR) + FAILURE);
      exit(FAILURE);
    }
  else
    {
      catch_signal(&signal_on_exec);
      trace_son(&stop_son);
    }
  return (0);
}

int		trace_pid(char const *const pid_str)
{
  if (pid_str == NULL)
    return (dprintf(2, INV_ARG_P) - sizeof(INV_ARG_P) + FAILURE);
  if (((int)(g_pid = atoi(pid_str))) < 1)
    return (dprintf(2, ERR_PID_NEG) - sizeof(ERR_PID_NEG) + FAILURE);
  if (ptrace(PTRACE_ATTACH, g_pid, NULL, NULL) == -1)
    {
      perror(NULL);
      return (dprintf(2, "%s%u\n", PTRACE_ATTACH_ERR, g_pid)
	      - sizeof(PTRACE_ATTACH_ERR) - strlen(pid_str) - 1 + FAILURE);
    }
  catch_signal(&signal_on_p);
  trace_son(&detach_process);
  printf("Process %u attached\n", g_pid);
  return (0);
}

#undef ELF
#undef PID
#undef _GNU_SOURCE

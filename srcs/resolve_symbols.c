#define PID
#include <unistd.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ftrace.h"

static size_t	find_header(size_t addr)
{
  unsigned long	read;
  int		done = 0;
  char		*ptr;

  ptr = (char *)&read;
  while (!done)
  {
    read = ptrace(PTRACE_PEEKDATA, g_pid, (void *)addr, (void *)0);
    if (read == (unsigned long)-1)
    {
      return (0);
    }
    if (!strncmp(MAGIC_NUMBER, ptr, 4))
    {
      return (addr);
    }
    addr -= 1;
  }
  return (0);
}

static int		resolve_symbol_lib(size_t addr, t_symbol *symbols)
{
  register size_t	i;

  i = 0;
  if (!symbols || !addr)
    return (0);
  while (symbols[i].name)
  {
    if (symbols[i].value == addr)
    {
      printf(" %s", symbols[i].name);
      return (1);
    }
    i += 1;
  }
  return (0);
}

static void			get_lib(int fd, size_t addr, size_t base_elf)
{
  static t_list_lib_symbols	*list_lib = 0;
  char				*str;
  char				*lib;
  char				*tmp;
  uint64_t			begin;
  uint64_t			end;
  t_symbol			*sym;

  for (str = get_next_line(fd), lib = 0; str; str = get_next_line(fd))
  {
    begin = strtol(str, 0, 16);
    end = ((tmp = strchr(str, '-')) && *tmp) ? (strtol(tmp + 1, 0, 16)) : (0);
    if (((begin < addr && addr < end) || begin == base_elf)
	&& (lib = strchr(str, '/')))
    {
      if (!(sym = get_symbols_by_lib(list_lib, lib))
	  && (sym = get_symbols_lib(lib)))
  	push_lib(&list_lib, lib, sym);
      if (resolve_symbol_lib(addr - begin, sym) && printf(" (%s)", lib))
  	break ;
    }
    free(str);
  }
}

static void	get_lib_file(size_t addr, size_t base_elf)
{
  char		maps[256];
  int		fd;

  snprintf(maps, sizeof(maps) - 1, "/proc/%u/maps", g_pid);
  if ((fd = open(maps, O_RDONLY)) < 0)
    return ;
  get_lib(fd, addr, base_elf);
  close(fd);
}

void		resolve_symbol(size_t addr, t_symbol *symbols, int is_a_call)
{
  size_t	i;
  size_t	base_elf;
  int		found;

  i = - 1;
  if (!symbols || !addr || !is_a_call)
    return ;
  found = 0;
  while (symbols[++i].value
  	 || symbols[i].type
  	 || symbols[i].name)
  {
    if (symbols[i].value == addr)
    {
      printf(" ..... %s\n", symbols[i].name);
      found = 1;
    }
  }
  if (!found)
  {
    printf(" .....");
    base_elf = find_header(addr);
    get_lib_file(addr, base_elf);
    printf("\n");
  }
}

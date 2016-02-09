#define _GNU_SOURCE
#define PID
#define ELF
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <elf.h>
#include <stdio.h>
#include "ftrace.h"

static off_t	calcfilesize(int const fd, char const *const name)
{
  off_t		i;

  if ((i = lseek(fd, 0, SEEK_END)) == -1)
    {
      dprintf(2, "Error : File %s can't know size.\n", name);
      return (FAILURE);
    }
  return (i);
}

static int		load_file(t_infos *const infos,
				  int (*fct)())
{
  int		fd;

  /* printf("%s\n", __func__); */
  if ((fd = open(infos->exe, O_RDONLY)) == -1)
    {
      dprintf(2, "Error : Can't open file %s.\n", infos->exe);
      return (FAILURE);
    }
  if ((infos->filesize = calcfilesize(fd, infos->exe)) == FAILURE)
    {
      if (close(fd) == -1)
	dprintf(2, "%s", ERR_CLOSE);
      return (FAILURE);
    }
  if ((infos->data = mmap(NULL, infos->filesize, PROT_READ, MAP_SHARED, fd,
			  0)) == MAP_FAILED)
    return (dprintf(2, "%s", MMAP_FAILED) - sizeof(MMAP_FAILED) + FAILURE);
  if (close(fd) == -1)
    dprintf(2, "Error : Can't close the file %s.\n", infos->exe);
  return (parse_file(infos, fct));
}

int		free_infos(t_infos *const infos,
			   char const *const display,
			   int const ret,
			   int (*fct)())
{
  if (fct != NULL)
    fct();
  free(infos->path);
  free(infos->exe);
  if (display != NULL)
    dprintf(2, "%s", display);
  printf("%s\n", __func__);
  munmap(infos->data, infos->filesize);
  return (ret);
}

int		load_infos(t_infos *const infos,
			     int (*fct_failed)())
{
  if (asprintf(&infos->path, "/proc/%u", g_pid) == -1 ||
      asprintf(&infos->exe, "%s/exe", infos->path) == -1)
    {
      dprintf(2, "%s", MEMORY_ERR);
      return (fct_failed());
    }
  if (load_file(infos, fct_failed) == FAILURE)
    return (fct_failed());
  return (SUCCESS);
}
#undef ELF
#undef _GNU_SOURCE
#undef PID

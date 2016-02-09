#include <strings.h>
#include <string.h>
#include "ftrace.h"

static t_list_lib_symbols	*create_elem(char *libname,
					     t_symbol *symbols)
{
  t_list_lib_symbols		*new;

  if ((new = malloc(sizeof(*new))))
  {
    new->libname = strdup(libname);
    if (!new->libname)
    {
      free(new);
      return (0);
    }
    new->symbols = symbols;
    new->next = 0;
  }
  return (new);
}

int			push_lib(t_list_lib_symbols **list,
				 char *libname,
				 t_symbol *symbols)
{
  t_list_lib_symbols	*tmp;
  t_list_lib_symbols	*new;

  if (!(new = create_elem(libname, symbols)))
    return (-1);
  if ((tmp = *list))
  {
    new->next = tmp;
    *list = new;
    /* new->next = tmp; */
  }
  else
  {
    *list = new;
  }
  return (0);
}

t_symbol	*get_symbols_by_lib(t_list_lib_symbols *list,
				    char *libname)
{
  while (list)
  {
    /* printf("0: '%s' 1: '%s'\n", libname, list->libname); */
    if (!strcmp(libname, list->libname))
    {
      /* printf("FOUND: '%s'\n", libname); */
      return (list->symbols);
    }
    list = list->next;
  }
  return (0);
}

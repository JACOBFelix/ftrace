#include <string.h>
#include "ftrace.h"

static t_list_functions	*create_function(char *label)
{
  t_list_functions	*new;

  if ((new = malloc(sizeof(*new))))
  {
    new->label = strdup(label);
    if (!new->label)
    {
      free(new);
      return (0);
    }
    new->next = 0;
  }
  return (new);
}

int			push_function(t_list_functions **list, char *label)
{
  t_list_functions	*new;
  t_list_functions	*tmp;

  if (!(new = create_function(label)))
    return (-1);
  if ((tmp = *list))
  {
    new->next = tmp;
    *list = new;
  }
  else
  {
    *list = new;
  }
  return (0);
}

void			pop_function(t_list_functions **list)
{
  t_list_functions	*tmp;

  if (!list)
    return ;
  tmp = *list;
  if (!tmp)
    return ;
  tmp = tmp->next;
  free(*list);
  *list = tmp;
}

char			*get_function(t_list_functions *list)
{
  return ((list) ? (list->label) : (0));
}

#include <unistd.h>
#include <sys/ptrace.h>
#include <string.h>
#include "ftrace.h"

static int	print_word(long text)
{
  char		buffer[3];
  int		nb;

  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, 2, "%c", (char)text);
  if (((char *)&text)[1] != 0)
  {
    snprintf(buffer + 1, 2, "%c", ((char *)&text)[1]);
  }
  nb = printf("%s", buffer);
  return (nb);
}

int		print_char_ptr(pid_t pid,
			       size_t value)
{
  char		*s;
  size_t	i;
  long		text;
  char		*txt;
  int		nb;

  s = (char*)value;
  if (!s)
    return (0);
  nb = 0;
  nb += printf("\"");
  i = 0;
  txt = "abc";
  text = 0;
  while (i < 20 && txt[0])
  /* while (i < 20 && my_strlen(txt) > 1) */
  {
    text = ptrace(PTRACE_PEEKTEXT, pid, s, (void *)0);
    txt = (char *)&text;
    if (strlen(txt))
      nb += print_word(text);
    s += 2;
    i += 1;
  }
  nb += printf((i == 20) ? ("\"...") : ("\""));
  return (nb);
}

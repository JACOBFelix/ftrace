#include "ftrace.h"

int		print_int(pid_t pid __attribute__ ((unused)),
			  size_t value)
{
  return (printf("%d", (int)value));
}

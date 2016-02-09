#include "ftrace.h"

int		print_ssize_t(pid_t pid __attribute__ ((unused)),
			      size_t value)
{
  return (printf("%ld", (ssize_t)value));
}

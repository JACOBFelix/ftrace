#include "ftrace.h"

int		print_size_t(pid_t pid __attribute__ ((unused)),
			     size_t value)
{
  return (printf("%lu", (uint64_t)value));
}

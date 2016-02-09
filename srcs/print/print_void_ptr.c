#include "ftrace.h"

int		print_void_ptr(pid_t pid __attribute__ ((unused)),
			       size_t value)
{
  if (value)
  {
    return (printf("%p", (void *)value));
  }
  else
  {
    return (printf("0"));
  }
}

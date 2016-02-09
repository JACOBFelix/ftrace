#include "ftrace.h"

int		print_off_t(pid_t pid __attribute__ ((unused)),
			    size_t value)
{
  if (value)
  {
    return (printf("0x%lx", value));
  }
  else
  {
    return (printf("0"));
  }
}

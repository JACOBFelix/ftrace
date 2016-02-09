#include "stdio.h"
#include "ftrace.h"

void		print_space(int cara)
{
  if (cara)
  {
    while (cara < SPACES)
    {
      printf(".");
      cara += 1;
    }
    printf(" ");
  }
}

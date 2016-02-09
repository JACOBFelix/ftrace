#include <stdio.h>
#include "ftrace.h"

int			instruction_is_syscall(long instruction)
{
  unsigned short	type;

  type = (instruction & 0x000000000000ffff);
  if (type == 0x050FU || type == 0x80CDU || type == 0x340FU)
    return (type);
  if (type == 0x0350FU)
    printf("Sysexit\n");
  return (FAILURE);
}

int			instruction_is_ret(long instruction)
{
  unsigned char		type;

  type = (instruction & 0x00000000000000ff);
  if (type == 0xCAU || type == 0xCBU || type == 0xC2U || type == 0xC3U
      || type == 0xCFU)
    return (type);
  if ((instruction & 0xFFFF) == 0xC3F3)
    return (type);
  return (FAILURE);
}

/*
**	0xCF Interrupt Return    iret || iretd
**	0xCA Return from procedure retf
*/
int			instruction_is_call(long instruction)
{
  unsigned char		type;

  type = (instruction & 0x00000000000000ff);
  if (type == 0xE8 || type == 0xFFU || type == 0x9A || type == 0x41)
    return (type);
  return (FAILURE);
}

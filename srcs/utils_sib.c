#include <stdio.h>
#include "ftrace.h"

static unsigned int	get_sib_byte(unsigned long instr)
{
  return ((instr >> 16) & 0xFF);
}

static unsigned int	get_sib_dword(unsigned long instr)
{
  return ((instr >> 16) & 0xFFFFFFFF);
}

unsigned long		manage_byte(unsigned long instr, unsigned long *addr)
{
  char			dec;
  int			cara;

  dec = get_sib_byte(instr);
  cara = printf(" + byte(0x%X)", dec);
  (*addr) += (char)dec;
  return (cara);
}

unsigned long		manage_dword(unsigned long instr, unsigned long *addr)
{
  int			dec;
  int			cara;

  dec = get_sib_dword(instr);
  cara = printf(" + dword(0x%X)", dec);
  (*addr) += (int)dec;
  return (cara);
}

#include "ftrace.h"

char		get_size_sib(unsigned char rm_byte)
{
  if (rm_byte >= 0x50 && rm_byte <= 0x57)
    return (1);
  else if ((rm_byte >= 0x90 && rm_byte <= 0x97) || rm_byte == 0x15)
    return (4);
  return (0);
}

int		get_sib(unsigned long instr, char size_sib)
{
  instr >>= 8;
  return ((size_sib == 1) ? (instr & 0xFF) : (instr & 0xFFFFFFFF));
}

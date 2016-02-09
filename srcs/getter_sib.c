#include <stdio.h>
#include "ftrace.h"
#include "tab_sib.h"

unsigned char	get_base(unsigned char sib)
{
  return (sib & 0b111);
}

size_t		get_index_sib(unsigned char sib)
{
  size_t	i;

  for (i = 0; i < g_size_sib; i += 1)
    if (sib >= g_sib[i].sib_min && sib <= g_sib[i].sib_max)
      return (i);
  return (0);
}

size_t		get_index_base(unsigned char base, char rex)
{
  size_t	i;

  for (i = 0; i < g_size_sib_base; i += 1)
    if (base == g_sib_base[i].base
	&& ((rex == g_sib_base[i].rex) || g_prefix == 0x42))
      return (i);
  return (0);
}

unsigned long		get_value_index_scale(size_t index_sib,
					      char rex,
					      struct user_regs_struct regs)
{
  unsigned long		value;

  value = 0;
  if (!rex || g_prefix == 0x41)
  {
    if (g_sib[index_sib].get_reg)
      value = g_sib[index_sib].get_reg(regs);
  }
  else
  {
    if (g_sib[index_sib].get_reg_ext)
      value = g_sib[index_sib].get_reg_ext(regs);
  }
  value *= g_sib[index_sib].scale;
  return (value);
}

unsigned long	get_value_base(size_t index_base,
			       struct user_regs_struct regs)
{
  if (g_sib_base[index_base].get_reg)
    return (g_sib_base[index_base].get_reg(regs));
  return (0);
}

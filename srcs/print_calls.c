#define PID
#define ELF
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include "ftrace.h"

static const t_regs_call	g_regs_call[] =
{
  { 0, 1, "[rax]", 0x10, get_rax },
  { 0, 1, "[rcx]", 0x11, get_rcx },
  { 0, 1, "[rdx]", 0x12, get_rdx },
  { 0, 1, "[rbx]", 0x13, get_rbx },
  { 0, 1, 0000000, 0x14, NULL },
  { 0, 1, "[rip + 0x%lX]", 0x15, get_rip },
  { 0, 1, "[rsi]", 0x16, get_rsi },
  { 0, 1, "[rdi]", 0x17, get_rdi },
  { 0, 1, "[rax + byte(0x%X)]", 0x50 , get_rax },
  { 0, 1, "[rcx + byte(0x%X)]", 0x51 , get_rcx },
  { 0, 1, "[rdx + byte(0x%X)]", 0x52 , get_rdx },
  { 0, 1, "[rbx + byte(0x%X)]", 0x53 , get_rbx },
  { 0, 1, 00000000000000000000, 0x54 , NULL },
  { 0, 1, "[rbp + byte(0x%X)]", 0x55 , get_rbp },
  { 0, 1, "[rsi + byte(0x%X)]", 0x56 , get_rsi },
  { 0, 1, "[rdi + byte(0x%X)]", 0x57 , get_rdi },
  { 0, 1, "[rax + dword(0x%X)]", 0x90 , get_rax },
  { 0, 1, "[rcx + dword(0x%X)]", 0x91 , get_rcx },
  { 0, 1, "[rdx + dword(0x%X)]", 0x92 , get_rdx },
  { 0, 1, "[rbx + dword(0x%X)]", 0x93 , get_rbx },
  { 0, 1, 000000000000000000000, 0x94 , NULL },
  { 0, 1, "[rbp + dword(0x%X)]", 0x95 , get_rbp },
  { 0, 1, "[rsi + dword(0x%X)]", 0x96 , get_rsi },
  { 0, 1, "[rdi + dword(0x%X)]", 0x97 , get_rdi },
  { 0, 0, "rax", 0xD0, get_rax },
  { 0, 0, "rcx", 0xD1, get_rcx },
  { 0, 0, "rdx", 0xD2, get_rdx },
  { 0, 0, "rbx", 0xD3, get_rbx },
  { 0, 0, "rsp", 0xD4, get_rsp },
  { 0, 0, "rbp", 0xD5, get_rbp },
  { 0, 0, "rsi", 0xD6, get_rsi },
  { 0, 0, "rdi", 0xD7, get_rdi },
  { 1, 1, "[r8]", 0x10, get_r8 },
  { 1, 1, "[r9]", 0x11, get_r9 },
  { 1, 1, "[r10]", 0x12, get_r10 },
  { 1, 1, "[r11]", 0x13, get_r11 },
  { 1, 1, 0000000, 0x14, NULL },
  { 1, 1, "[rip + 0x%lX]", 0x15, get_rip },
  { 1, 1, "[r14]", 0x16, get_r14 },
  { 1, 1, "[r15]", 0x17, get_r14 },
  { 1, 1, "[r8 + byte(0x%X)]", 0x50 , get_r8 },
  { 1, 1, "[r9 + byte(0x%X)]", 0x51 , get_r9 },
  { 1, 1, "[r10 + byte(0x%X)]", 0x52 , get_r10 },
  { 1, 1, "[r11 + byte(0x%X)]", 0x53 , get_r11 },
  { 1, 1, 00000000000000000000, 0x54 , NULL },
  { 1, 1, "[r13 + byte(0x%X)]", 0x55 , get_r13 },
  { 1, 1, "[r14 + byte(0x%X)]", 0x56 , get_r14 },
  { 1, 1, "[r15 + byte(0x%X)]", 0x57 , get_r15 },
  { 1, 1, "[r8 + dword(0x%X)]", 0x90 , get_r8 },
  { 1, 1, "[r9 + dword(0x%X)]", 0x91 , get_r9 },
  { 1, 1, "[r10 + dword(0x%X)]", 0x92 , get_r10 },
  { 1, 1, "[r11 + dword(0x%X)]", 0x93 , get_r11 },
  { 1, 1, 000000000000000000000, 0x94 , NULL },
  { 1, 1, "[r13 + dword(0x%X)]", 0x95 , get_r13 },
  { 1, 1, "[r14 + dword(0x%X)]", 0x96 , get_r14 },
  { 1, 1, "[r15 + dword(0x%X)]", 0x97 , get_r15 },
  { 1, 0, "r8", 0xD0, get_r8 },
  { 1, 0, "r9", 0xD1, get_r9 },
  { 1, 0, "r10", 0xD2, get_r10 },
  { 1, 0, "r11", 0xD3, get_r11 },
  { 1, 0, "r12", 0xD4, get_r12 },
  { 1, 0, "r13", 0xD5, get_r13 },
  { 1, 0, "r14", 0xD6, get_r14 },
  { 1, 0, "r15", 0xD7, get_r15 },
};

static const size_t	g_size_regs_call = sizeof(g_regs_call)
  / sizeof(g_regs_call[0]);

static char	is_a_call(unsigned char rm_byte)
{
  size_t	i;

  for (i = 0; i < g_size_regs_call; i += 1)
  {
    if (rm_byte == g_regs_call[i].rm_byte)
      return (1);
  }
  return (0);
}

static int	print_opcodes(char *fmt, ...)
{
  char		buffer[256];
  va_list	ap;

  if (fmt)
  {
    memset(buffer, 0, sizeof(buffer));
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
    va_end(ap);
    return (printf("call %s: ", buffer));
  }
  else
  {
    (void)buffer;
    return (0);
  }
}

static unsigned long	get_addr_func(unsigned long instr,
				      size_t i,
				      char rex,
				      struct user_regs_struct regs)
{
  unsigned long		addr;
  unsigned int		sib;
  char			size_sib;

  size_sib = get_size_sib(instr & 0xFF);
  if (!g_regs_call[i].get_reg)
    return (resolve_sib(instr, rex, regs));
  else if (!g_regs_call[i].indirect_value)
    return (g_regs_call[i].get_reg(regs));
  else
  {
    sib = get_sib(instr, size_sib);
    addr = g_regs_call[i].get_reg(regs);
    if (size_sib == 4)
      addr += (int)sib;
    else if (size_sib == 1)
      addr += (char)sib;
    if ((instr & 0xFF) == 0x15)
      addr += 6 + rex;
    return (ptrace(PTRACE_PEEKTEXT, g_pid, addr, (void *)0));
  }
}

static unsigned long	print_rm_byte_call(unsigned long instr,
					   char size_sib,
					   char rex,
					   struct user_regs_struct regs)
{
  size_t		i;
  unsigned int		sib;
  unsigned long		addr_func;
  int			cara;

  addr_func = 0;
  for (i = 0; i < g_size_regs_call; i += 1)
  {
    if ((instr & 0xFF) == g_regs_call[i].rm_byte && rex == g_regs_call[i].rex)
    {
      if (size_sib)
      {
	sib = get_sib(instr, size_sib);
	cara = print_opcodes(g_regs_call[i].name, sib);
      }
      else
	cara = print_opcodes(g_regs_call[i].name);
      print_space(cara);
      printf("0x%016lX", addr_func = get_addr_func(instr, i, rex, regs));
      /* printf("\n"); */
      return (addr_func);
    }
  }
  return (0);
}

size_t		print_call(unsigned long instr,
			   struct user_regs_struct regs,
			   int *a_call)
{
  size_t	addr;
  unsigned char	rex;
  int		cara;

  g_prefix = 0;
  addr = 0;
  if ((uint8_t)instr == 0xE8 && (++(*a_call)))
  {
    instr = my_bswap(((my_bswap(instr) << 8) >> 8) >> 24) >> 32;
    cara = printf("call (relative):");
    print_space(cara);
    printf("0x%016lX", (addr = (unsigned long)(regs.rip + 5) + (int)instr));
  }
  else if (((instr & 0xFF) == 0xFF) || ((instr & 0xFFFF) == 0xFF41))
  {
    rex = 0;
    if (((instr & 0xFF) == 0x41 || (instr & 0xFF) == 0x43) && ++rex)
    {
      g_prefix = (instr & 0xFF);
      instr >>= 8;
    }
    instr >>= 8;
    if (is_a_call(instr & 0xFF) && (++(*a_call)))
      addr = print_rm_byte_call(instr, get_size_sib(instr & 0xFF), rex, regs);
  }
  return (addr);
}

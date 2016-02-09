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

const t_sib	g_sib[] =
{
  { "rax*1 + %s", "r8*1  + %s", 1, 0x00, 0x07, get_rax, get_r8 },
  { "rcx*1 + %s", "r9*1  + %s", 1, 0x08, 0x0F, get_rcx, get_r9 },
  { "rdx*1 + %s", "r10*1 + %s", 1, 0x10, 0x17, get_rdx, get_r10 },
  { "rbx*1 + %s", "r11*1 + %s", 1, 0x18, 0x1F, get_rbx, get_r11 },
  { "000*1 + %s", "r12*1 + %s", 1, 0x20, 0x27, 0000000, get_r12 },
  { "rbp*1 + %s", "r13*1 + %s", 1, 0x28, 0x2F, get_rbp, get_r13 },
  { "rsi*1 + %s", "r14*1 + %s", 1, 0x30, 0x37, get_rsi, get_r14 },
  { "rdi*1 + %s", "r15*1 + %s", 1, 0x38, 0x3F, get_rdi, get_r15 },
  { "rax*2 + %s", "r8*2  + %s", 2, 0x40, 0x47, get_rax, get_r8 },
  { "rcx*2 + %s", "r9*2  + %s", 2, 0x48, 0x4F, get_rcx, get_r9 },
  { "rdx*2 + %s", "r10*2 + %s", 2, 0x50, 0x57, get_rdx, get_r10 },
  { "rbx*2 + %s", "r11*2 + %s", 2, 0x58, 0x5F, get_rbx, get_r11 },
  { "000*2 + %s", "r12*2 + %s", 2, 0x60, 0x67, 0000000, get_r12 },
  { "rbp*2 + %s", "r13*2 + %s", 2, 0x68, 0x6F, get_rbp, get_r13 },
  { "rsi*2 + %s", "r14*2 + %s", 2, 0x70, 0x77, get_rsi, get_r14 },
  { "rdi*2 + %s", "r15*2 + %s", 2, 0x78, 0x7F, get_rdi, get_r15 },
  { "rax*4 + %s", "r8*4  + %s", 4, 0x80, 0x87, get_rax, get_r8 },
  { "rcx*4 + %s", "r9*4  + %s", 4, 0x88, 0x8F, get_rcx, get_r9 },
  { "rdx*4 + %s", "r10*4 + %s", 4, 0x90, 0x97, get_rdx, get_r10 },
  { "rbx*4 + %s", "r11*4 + %s", 4, 0x98, 0x9F, get_rbx, get_r11 },
  { "000*4 + %s", "r12*4 + %s", 4, 0xA0, 0xA7, 0000000, get_r12 },
  { "rbp*4 + %s", "r13*4 + %s", 4, 0xA8, 0xAF, get_rbp, get_r13 },
  { "rsi*4 + %s", "r14*4 + %s", 4, 0xB0, 0xB7, get_rsi, get_r14 },
  { "rdi*4 + %s", "r15*4 + %s", 4, 0xB8, 0xBF, get_rdi, get_r15 },
  { "rax*8 + %s", "r8*8  + %s", 8, 0xC0, 0xC7, get_rax, get_r8 },
  { "rcx*8 + %s", "r9*8  + %s", 8, 0xC8, 0xCF, get_rcx, get_r9 },
  { "rdx*8 + %s", "r10*8 + %s", 8, 0xD0, 0xD7, get_rdx, get_r10 },
  { "rbx*8 + %s", "r11*8 + %s", 8, 0xD8, 0xDF, get_rbx, get_r11 },
  { "000*8 + %s", "r12*8 + %s", 8, 0xE0, 0xE7, 0000000, get_r12 },
  { "rbp*8 + %s", "r13*8 + %s", 8, 0xE8, 0xEF, get_rbp, get_r13 },
  { "rsi*8 + %s", "r14*8 + %s", 8, 0xF0, 0xF7, get_rsi, get_r14 },
  { "rdi*8 + %s", "r15*8 + %s", 8, 0xF8, 0xFF, get_rdi, get_r15 },
};

const t_sib_base	g_sib_base[] =
{
  { "rax", 0, 0b000, get_rax },
  { "rcx", 0, 0b001, get_rcx },
  { "rdx", 0, 0b010, get_rdx },
  { "rbx", 0, 0b011, get_rcx },
  { "rsp", 0, 0b100, get_rsp },
  { "%lX", 0, 0b101, 0000000 },
  { "rsi", 0, 0b110, get_rsi },
  { "rdi", 0, 0b111, get_rdi },
  { " r8", 1, 0b000, get_r8 },
  { " r9", 1, 0b001, get_r9 },
  { "r10", 1, 0b010, get_r10 },
  { "r11", 1, 0b011, get_r11 },
  { "r12", 1, 0b100, get_r12 },
  { "%lX", 1, 0b101, 0000000 },
  { "r14", 1, 0b110, get_r14 },
  { "r15", 1, 0b111, get_r15 }
};

const size_t	g_size_sib = sizeof(g_sib) / sizeof(g_sib[0]);

const size_t	g_size_sib_base = sizeof(g_sib_base)
  / sizeof(g_sib_base[0]);

static int		print_sib(size_t index_sib,
				  size_t index_base,
				  char rex)
{
  char			buffer[256];

  memset(buffer, 0, sizeof(buffer));
  if (!rex || g_prefix == 0x41)
    snprintf(buffer, sizeof(buffer) - 1, "%s", g_sib[index_sib].name);
  else
    snprintf(buffer, sizeof(buffer) - 1, "%s", g_sib[index_sib].name_ext);
  return (printf(buffer, g_sib_base[index_base].name));
}

unsigned long		resolve_sib(unsigned long instr,
				    char rex,
				    struct user_regs_struct regs)
{
  unsigned char		sib;
  size_t		index_sib;
  size_t		index_base;
  unsigned long		addr;
  unsigned long		value;
  int			cara;

  sib = (instr >> 8) & 0xFF;
  index_sib = get_index_sib(sib);
  index_base = get_index_base(get_base(sib), rex);
  addr = get_value_index_scale(index_sib, rex, regs)
    + get_value_base(index_base, regs);
  cara = printf("call [");
  cara += print_sib(index_sib, index_base, rex);
  if ((instr & 0xFF) == 0x54)
    cara += manage_byte(instr, &addr);
  else if ((instr & 0xFF) == 0x94)
    cara += manage_dword(instr, &addr);
  cara += printf("]: ");
  print_space(cara);
  value = ptrace(PTRACE_PEEKTEXT, g_pid, addr, NULL);
  return (value);
}

#ifndef FTRACE_H_
# define FTRACE_H_

# include <stdlib.h>
# include <sys/user.h>
# include <elf.h>
# include <sys/types.h>
# include "get_next_line.h"

# define FAILURE -1
# define SUCCESS 0
# define UNUSED __attribute__((unused))
# define NEED_ARG "./ftrace [-p PID] or [command argument(s)].\n"
# define INV_ARG_P "Error -p argument invalid : need a PID.\n"
# define INV_ARG "Bad argument : Can't exec the binary file : "
# define INV_PID "Error -p pid invalid.\n"
# define GETENVPWD_ERR "Error can not have the path of the current directory.\n"
# define GETENVPATH_ERR "Error can not have the variable PATH in environement.\n"
# define MEMORY_ERR "Error during allocation of memory.\n"
# define FORK_ERR "Error with fork : Can not create the process to trace.\n"
# define EXECVE_ERR "Error with execve : can't not execute the binary argument.\n"
# define PTRACE_ATTACH_ERR "Error with ptrace : can not attach the process with id :"
# define ACCESS_EXEC_ERR "Error : No right to exec file : "
# define ERR_PTRACE_PEEKTEXT "Error : ptrace_peektext failed.\n"
# define ERR_PTRACE_TRACEME "Error : ptrace_traceme failed on process to traced.\n"
# define ERR_KILL_SON "Error can't kill the process traced with pid : "
# define MMAP_FAILED "Error : mmap failed.\n"
# define ERR_CLOSE "Error : close failed.\n"
# define ERR_PID_NEG "Error : Pid can't not be negative.\n"
# define ERR_ELF_HEADER "Error : Header elf is invalid.\n"
# define ERR_ELF32 "Error : Elf format 32 bits is not supported.\n"
# define ERR_UNDEFCLASS_ELF "Error : Elf format is unknown.\n"
# define ELF_INVALID "Error : format executable file is corrumped.\n"
#  ifdef ELF
#   include <elf.h>

typedef struct	s_infos
{
  char		*path;
  char		*exe;
  ssize_t	size;
  void		*data;
  off_t		filesize;
  Elf64_Ehdr	*ehdr;
  Elf64_Shdr	*shdr;
  char		*strtab;
}		t_infos;

/*
**		parse_elf.c
*/
int		parse_file(t_infos *const,
			   int (*fct)());
/*
**		load_infos.c
*/
int		free_infos(t_infos *,
			   char const *const,
			   int const,
			   int (*fct)());
int		load_infos(t_infos *const,
			   int (*)());

#  endif /* !ELF */

#  ifdef PID
pid_t		g_pid;
#  endif /* !PID */
/*
**		exec.c
*/
int		exec(char const *const,
		     char const *const [],
		     char const *const []);
int		trace_pid(char const *const);
/*
**		catch_signal.c
*/
void		signal_on_p(int);
void		signal_on_exec(int);
void		catch_signal(void(*fct)(int));
#  ifdef PID
/*
**		wait.c
*/
int		my_wait(pid_t);
#  endif /* !PID */
/*
**		type_instruction.c
*/
int		instruction_is_syscall(long);
int		instruction_is_ret(long);
int		instruction_is_call(long);
/*
**		stop_trace.c
*/
int		stop_son();
int		detach_process();

void		print_space(int);

#  define SPACES	(40)
#  define MAGIC_NUMBER	"\177ELF"
#  define ELF_SHDR (Elf64_Shdr *)((void *)elf->ehdr + elf->ehdr->e_shoff)
#  define SYM_START (Elf64_Sym *)((void *)elf->ehdr + elf->shdr[i].sh_offset)
#  define SYM_END (Elf64_Sym *)((void *)elf->sym.start + elf->shdr[i].sh_size)
#  define SYM_STR (char *)elf->ehdr + elf->shdr[elf->shdr[i].sh_link].sh_offset
#  define SIZE(x)	(sizeof(x) / sizeof(*x))
#  define LOWER(x)	(x >= 'A' && x <= 'Z' ? x + 0x20 : x)

typedef struct	s_syscalls
{
  unsigned	num;
  char		*name;
  char		man_found;
  unsigned	nparams;
  int		type_return;
  int		param1;
  int		param2;
  int		param3;
  int		param4;
  int		param5;
  int		param6;
}		t_syscalls;

typedef struct	s_print_func
{
  char		*name;
  void		(*fct)(pid_t pid, struct user_regs_struct *regs, size_t ret);
}		t_print_func;

typedef struct		s_type
{
  char			ch;
  uint32_t		type;
  uint64_t		flags;
}			t_type;

typedef struct		s_sym
{
  Elf64_Sym const	*start;
  Elf64_Sym const	*end;
  char const		*strtab;
}			t_sym;

typedef struct		s_symbol
{
  unsigned int		value;
  char			type;
  char const		*name;
}			t_symbol;

typedef struct		s_elf
{
  Elf64_Ehdr const	*ehdr;
  Elf64_Shdr const	*shdr;
  unsigned int		len;
  int			sort;
  void			*end;
  t_sym			sym;
}			t_elf;

typedef struct	s_regs_call
{
  unsigned char	rex;
  unsigned char	indirect_value;
  char		*name;
  unsigned char	rm_byte;
  long		(*get_reg)(struct user_regs_struct);
}		t_regs_call;

typedef struct	s_sib
{
  char		*name;
  char		*name_ext;
  char		scale;
  unsigned char	sib_min;
  unsigned char	sib_max;
  long		(*get_reg)(struct user_regs_struct);
  long		(*get_reg_ext)(struct user_regs_struct);
}		t_sib;

typedef struct	s_sib_base
{
  char		*name;
  unsigned char	rex;
  unsigned char	base;
  long		(*get_reg)(struct user_regs_struct);
}		t_sib_base;

unsigned char	g_prefix;

/*
**		bswap.c
*/
unsigned long	my_bswap(unsigned long);

/*
**		get_regs0.c
*/
long		get_rax(struct user_regs_struct regs);
long		get_rbx(struct user_regs_struct regs);
long		get_rcx(struct user_regs_struct regs);
long		get_rdx(struct user_regs_struct regs);
long		get_rsp(struct user_regs_struct regs);

/*
**		get_regs1.c
*/
long		get_rbp(struct user_regs_struct regs);
long		get_rsi(struct user_regs_struct regs);
long		get_rdi(struct user_regs_struct regs);
long		get_r8(struct user_regs_struct regs);
long		get_r9(struct user_regs_struct regs);

/*
**		get_regs2.c
*/
long		get_r10(struct user_regs_struct regs);
long		get_r11(struct user_regs_struct regs);
long		get_r12(struct user_regs_struct regs);
long		get_r13(struct user_regs_struct regs);
long		get_r14(struct user_regs_struct regs);

/*
**		get_regs3.c
*/
long		get_r15(struct user_regs_struct regs);
long		get_rip(struct user_regs_struct regs);


/*
**		print_call.c
*/
size_t		print_call(unsigned long instr,
			    struct user_regs_struct regs,
			    int *a_call);

/*
**		get_sib.c
*/
char		get_size_sib(unsigned char rm_byte);
int		get_sib(unsigned long instr, char size_sib);

/*
**		resolve_sib.c
*/
unsigned long	resolve_sib(unsigned long instr,
			    char rex,
			    struct user_regs_struct regs);

/*
**		utils_sib.c
*/
unsigned long		manage_byte(unsigned long instr, unsigned long *addr);
unsigned long		manage_dword(unsigned long instr, unsigned long *addr);

/*
**		getters_sib.c
*/

# include <stdio.h>

unsigned char	get_base(unsigned char sib);
size_t		get_index_sib(unsigned char sib);
size_t		get_index_base(unsigned char base, char rex);
unsigned long	get_value_index_scale(size_t index_sib,
				      char rex,
				      struct user_regs_struct regs);
unsigned long	get_value_base(size_t index_base,
			       struct user_regs_struct regs);
t_symbol	*get_symbols(void);
t_symbol	*get_symbols_mem(size_t addr);
t_symbol	*get_symbols_lib(char *lib);


int		print_int(pid_t pid, size_t value);
int		print_size_t(pid_t pid, size_t value);
int		print_ssize_t(pid_t pid, size_t value);
int		print_off_t(pid_t pid, size_t value);
int		print_void_ptr(pid_t pid, size_t value);
int		print_char_ptr(pid_t pid, size_t value);

void		print_mmap(pid_t pid,
			   struct user_regs_struct *regs,
			   size_t ret);
void		print_mmap2(pid_t pid,
			    struct user_regs_struct *regs,
			    size_t ret);
void		print_access(pid_t pid,
			     struct user_regs_struct *regs,
			     size_t ret);
void		print_open(pid_t pid,
			   struct user_regs_struct *regs,
			   size_t ret);
void		print_read(pid_t pid,
			   struct user_regs_struct *regs,
			   size_t ret);
void		print_mprotect(pid_t pid,
			       struct user_regs_struct *regs,
			       size_t ret);
void		print_generic(pid_t pid,
			      struct user_regs_struct *regs,
			      size_t return_value,
			      int num);
size_t		my_strlen(char *s);

typedef struct	s_arch_registers
{
  unsigned long	_32;
  unsigned long	_64;
}		t_arch_registers;

typedef struct	s_func
{
  int		type;
  int		(*fct)(pid_t pid, size_t value);
}		t_func;

size_t				get_param(struct user_regs_struct *regs,
					  int nparam);
int		is_syscall_defined(unsigned int const num);
void		print_sspace(int nb);
void		disp_syscall(pid_t pid,
			     struct user_regs_struct *regs,
			     unsigned num,
			     size_t return_value);

typedef struct	s_list_lib_symbols
{
  char				*libname;
  t_symbol			*symbols;
  struct s_list_lib_symbols	*next;
}		t_list_lib_symbols;

t_symbol	*get_symbols_by_lib(t_list_lib_symbols *list,
				    char *libname);
int			push_lib(t_list_lib_symbols **list,
				 char *libname,
				 t_symbol *symbols);
void		resolve_symbol(size_t addr, t_symbol *symbols, int is_a_call);

typedef struct			s_list_functions
{
  char				*label;
  struct s_list_functions	*next;
}				t_list_functions;

int			push_function(t_list_functions **list, char *label);
void			pop_function(t_list_functions **list);
char			*get_function(t_list_functions *list);
int			stock_fd(int fd);

#endif /* !FTRACE_H_ */

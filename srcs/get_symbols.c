#define PID

#include <elf.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "ftrace.h"

static void	swap(t_symbol *const a, t_symbol *const b)
{
  t_symbol	c;

  memcpy(&c, a, sizeof(c));
  memcpy(a, b, sizeof(c));
  memcpy(b, &c, sizeof(c));
}

static int	yolo_cmp(char const *a, char const *b)
{
  while (strchr("_@", *a))
    ++a;
  while (strchr("_@", *b))
    ++b;
  return (strcasecmp(a, b));
}

void		sort_sym(t_symbol *const sym_list, int const sort)
{
  register int	i;
  register int	j;

  i = -1;
  if (!sym_list)
    return ;
  while (sym_list[++i].value
	 || sym_list[i].type
	 || sym_list[i].name)
    {
      j = i - 1;
      while (sym_list[++j].value
	     || sym_list[j].type
	     || sym_list[j].name)
	if (sort == 24)
	  {
	    if (strcmp(sym_list[i].name, sym_list[j].name) > 0)
	      swap(&sym_list[i], &sym_list[j]);
	  }
	else if (yolo_cmp(sym_list[i].name, sym_list[j].name) > 0)
	  swap(&sym_list[i], &sym_list[j]);
      ++j;
    }
}

char			get_type(t_elf *const elf, Elf64_Sym const *const sym)
{
  register int		i;
  static t_type const	types[] = {{'B', SHT_NOBITS, SHF_ALLOC | SHF_WRITE},
				   {'R', SHT_PROGBITS, SHF_ALLOC},
				   {'D', SHT_PROGBITS, SHF_ALLOC | SHF_WRITE},
				   {'U', SHT_NULL, 0},
				   {'T', SHT_PROGBITS,
				   SHF_ALLOC | SHF_EXECINSTR},
				   {'R', SHT_PROGBITS, 0}};

  if (sym->st_shndx == SHN_UNDEF)
    return ('U');
  if (sym->st_shndx == SHN_COMMON)
    return ('C');
  if (sym->st_shndx == SHN_ABS)
    return ('A');
  i = -1;
  while ((unsigned)++i < SIZE(types))
    {
      if ((void *)&elf->shdr[sym->st_shndx] > elf->end)
	return (fprintf(stderr, "Error : invalid file\n") * 0 - 1);
      if (elf->shdr[sym->st_shndx].sh_type == types[i].type
	  && elf->shdr[sym->st_shndx].sh_flags == types[i].flags)
	return (types[i].ch);
    }
  return ('?');
}

int	fill_symbol(t_elf *const elf,
		    Elf64_Sym const *const sym,
		    t_symbol *const symbol)
{
  symbol->value = (unsigned int)sym->st_value;
  symbol->type = get_type(elf, sym);
  if (ELF64_ST_BIND(sym->st_info) == STB_LOCAL)
    symbol->type = LOWER(symbol->type);
  if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
    symbol->type = 'w';
  symbol->name = &elf->sym.strtab[sym->st_name];
  return (1);
}

static t_symbol		*get_sym(t_elf *const elf)
{
  t_symbol		*sym_list;
  Elf64_Sym const	*tmp;
  register int		i;

  if (!elf->sym.end || !elf->sym.start)
    return (0);
  if (!(sym_list = malloc(sizeof(t_symbol) * (elf->sym.end - elf->sym.start))))
    {
      perror("malloc");
      return (NULL);
    }
  memset(sym_list, 0, sizeof(t_symbol) *(elf->sym.end - elf->sym.start));
  tmp = elf->sym.start;
  i = 0;
  while (tmp < elf->sym.end)
    {
      if (tmp->st_info != STT_FILE && tmp->st_info != STT_SECTION
	  && tmp->st_info != STT_NOTYPE)
	{
	  if ((void *)&elf->sym.strtab[tmp->st_name] > elf->end)
	    return (fprintf(stderr, "Error : invalid file\n") * 0 + NULL);
	  if (fill_symbol(elf, tmp, &sym_list[i++]) == -1)
	    return (NULL);
	}
      ++tmp;
    }
  return (sym_list);
}

static t_symbol	*print_sym(t_elf *const elf)
{
  t_symbol	*sym_list;
  register int	i;

  if (!(sym_list = get_sym(elf)))
    return (0);
  sort_sym(sym_list, 0);
  i = -1;
  while (sym_list[++i].value
	 || sym_list[i].type
	 || sym_list[i].name)
    {
      /* if (sym_list[i].value) */
      /* 	printf("%016x %c %s\n", sym_list[i].value, */
      /* 	       sym_list[i].type, sym_list[i].name); */
      /* else */
      /* 	printf("%18c %s\n", sym_list[i].type, sym_list[i].name); */
    }
  /* free(sym_list); */
  return (sym_list);
}

t_symbol	*run_elf(t_elf *const elf)
{
  register int	i;

  if ((void *)(elf->shdr = ELF_SHDR) > elf->end)
    return ((t_symbol *)0);
  i = -1;
  while (++i != elf->ehdr->e_shnum)
    {
      if ((void *)&(elf->shdr[i]) > elf->end)
	return ((t_symbol *)0);
      if (elf->shdr[i].sh_type == SHT_SYMTAB)
	{
	  if ((void *)(elf->sym.start = SYM_START) > elf->end
	      || (void *)(elf->sym.end = SYM_END) > elf->end
	      || (void *)(elf->sym.strtab = SYM_STR) > elf->end)
	    return ((t_symbol *)0);
	}
    }
  if (elf->sym.end < elf->sym.start)
  {
    printf("No symbols\n");
    return ((t_symbol *)0);
  }
  return (print_sym(elf));
}

static int	check_magic_number(t_elf *const elf)
{
  if (elf->len < sizeof(Elf64_Ehdr)
      || memcmp(elf->ehdr, MAGIC_NUMBER, sizeof(MAGIC_NUMBER) - 1))
    {
      fprintf(stderr, "File format not recognized\n");
      return (0);
    }
  return (1);
}

int	parse_elf(char const *const file, t_elf *const elf)
{
  char	err[300];
  int	fd;

  if ((fd = open(file, O_RDONLY)) == -1)
    {
      sprintf(err, "open \"%s\"", file);
      perror(err);
      return (0);
    }
  if ((elf->len = lseek(fd, 0, SEEK_END)) == (unsigned)-1)
    {
      perror("lseek");
      return (0);
    }
  if (!(elf->ehdr = mmap(NULL, elf->len, PROT_READ, MAP_SHARED, fd, 0)))
    {
      perror("mmap");
      return (0);
    }
  close(fd);
  elf->end = (void *)elf->ehdr + elf->len;
  return (check_magic_number(elf));
}

t_symbol	*get_symbols(void)
{
  char		file[256];
  t_elf		elf;
  t_symbol	*symbols;

  memset(&elf, 0, sizeof(elf));
  memset(file, 0, sizeof(file));
  snprintf(file, sizeof(file) - 1, "/proc/%u/exe", g_pid);
  usleep(200000);
  if (!parse_elf(file, &elf))
    return (0);
  if (!(symbols = run_elf(&elf)))
    return (0);
  munmap(&elf.ehdr, elf.len);
  return (symbols);
}

t_symbol	*get_symbols_lib(char *lib)
{
  t_elf		elf;
  t_symbol	*symbols;

  memset(&elf, 0, sizeof(elf));
  if (!parse_elf(lib, &elf))
    return (0);
  if (!(symbols = run_elf(&elf)))
    return (0);
  return (symbols);
}

t_symbol	*get_symbols_mem(size_t addr)
{
  t_elf		elf;
  t_symbol	*symbols;

  memset(&elf, 0, sizeof(elf));
  elf.ehdr = (void *)addr;
  elf.len = 0x300000;
  elf.end = (void *)elf.ehdr + elf.len;
  printf("start: %p\n", elf.ehdr);
  printf("end:   %p\n", elf.end);
  symbols = run_elf(&elf);
  return (symbols);
}

#undef PID

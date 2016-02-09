#define ELF
#include <stdio.h>
#include "ftrace.h"

static int	check_header(t_infos *const infos)
{
  if (infos->data + sizeof(Elf64_Ehdr) > infos->data + infos->filesize ||
      infos->ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
      infos->ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
      infos->ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
      infos->ehdr->e_ident[EI_MAG3] != ELFMAG3 ||
      infos->ehdr->e_type != ET_EXEC)
    return (dprintf(2, "%s", ERR_ELF_HEADER) - sizeof(ERR_ELF_HEADER)
	    + FAILURE);
  if (infos->ehdr->e_ident[EI_CLASS] == ELFCLASS32)
    return (dprintf(2, "%s", ERR_ELF32) - sizeof(ERR_ELF32) + FAILURE);
  if (infos->ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    return (dprintf(2, "%s", ERR_UNDEFCLASS_ELF) - sizeof(ERR_UNDEFCLASS_ELF)
	    + FAILURE);
  return (SUCCESS);
}

int		parse_file(t_infos *const infos UNUSED, int (*fct)())
{
  infos->ehdr = (Elf64_Ehdr *)infos->data;
  if (check_header(infos) == FAILURE)
    return (fct());
  if (((void *)(infos->shdr = (Elf64_Shdr *)
		(infos->data + infos->ehdr->e_shoff)))
      > (void *)(infos->data + infos->filesize))
    {
      dprintf(2, "%s", ELF_INVALID);
      return (fct());
    }
  if ((infos->strtab = (char *)
       (infos->data + infos->shdr[infos->ehdr->e_shstrndx].sh_offset))
      > (char *)(infos->data + infos->filesize))
    {
      dprintf(2, "%s", ELF_INVALID);
      return (fct());
    }
  return (SUCCESS);
}
#undef ELF

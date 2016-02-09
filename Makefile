##
## Makefile for Makefile in /home/jacob_f/gitlab/Ftrace
##
## Made by felix jacob
## Login   <jacob_f@epitech.net>
##
## Started on  Wed May 27 09:41:48 2015 felix jacob
## Last update Wed Jun 17 00:20:14 2015 chapui_s
##

CC=		gcc -ggdb

RM=		rm -rf

NAME=		strace

DIR=		./srcs

SRCS=		$(DIR)/main.c \
		$(DIR)/exec.c \
		$(DIR)/catch_signal.c \
		$(DIR)/wait.c \
		$(DIR)/type_instruction.c \
		$(DIR)/load_infos.c \
		$(DIR)/parse_elf.c \
		$(DIR)/get_regs0.c \
		$(DIR)/get_regs1.c \
		$(DIR)/get_regs2.c \
		$(DIR)/get_regs3.c \
		$(DIR)/print_calls.c \
		$(DIR)/get_sib.c \
		$(DIR)/resolve_sib.c \
		$(DIR)/print_spaces.c \
		$(DIR)/utils_sib.c \
		$(DIR)/getter_sib.c \
		$(DIR)/get_symbols.c \
		$(DIR)/get_next_line.c \
		$(DIR)/disp_syscalls.c \
		$(DIR)/nums_syscalls.c \
		$(DIR)/bswap.c \
		$(DIR)/print/print_char_ptr.c \
		$(DIR)/print/print_int.c \
		$(DIR)/print/print_off_t.c \
		$(DIR)/print/print_size_t.c \
		$(DIR)/print/print_ssize_t.c \
		$(DIR)/print/print_void_ptr.c \
		$(DIR)/functions/access.c \
		$(DIR)/functions/mmap.c \
		$(DIR)/functions/mprotect.c \
		$(DIR)/functions/open.c \
		$(DIR)/functions/print_generic.c \
		$(DIR)/functions/read.c \
		$(DIR)/my_strlen.c \
		$(DIR)/list_lib_symbols.c \
		$(DIR)/get_params.c \
		$(DIR)/resolve_symbols.c \
		$(DIR)/list_functions.c \
		$(DIR)/stop_trace.c

OBJS=		$(SRCS:.c=.o)

CFLAGS+=	-Wall -W -Wextra -Werror -I ./includes -O3

NAME=		ftrace

GEN=		generate


%.o:		%.c
		@printf "[\033[0;32mCompiled\033[0m] % 35s\n" $< | tr ' ' '.'
		@$(CC) -c -o $@ $< $(CFLAGS)

all:		$(GEN) $(NAME)

$(GEN):
		@printf "[\033[0;36mGenerating\033[0m] % 33s\n" "64bits_syscalls" | tr ' ' '.'
		@./get_syscalls.sh /usr/include/asm/unistd_64.h


$(NAME):	$(OBJS)
		@$(CC) $(OBJS) -o $(NAME) $(CFLAGS)
		@printf "[\033[0;34mAssembled\033[0m] % 34s\n" $(NAME) | tr ' ' '.'
clean:
		@$(RM) $(OBJS)
		@printf "[\033[0;31mDeleted\033[0m] % 36s\n" $(OBJS) | tr ' ' '.'


fclean:		clean
		@$(RM) $(NAME)
		@printf "[\033[0;35mRemoved\033[0m] % 36s\n" $(NAME) | tr ' ' '.'

re:		fclean all

.PHONY:		all clean fclean re

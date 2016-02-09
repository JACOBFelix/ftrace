#ifndef GET_NEXT_LINE_H_
# define GET_NEXT_LINE_H_

# define BUF_SIZE	42

typedef struct	s_fd
{
  char		*str;
  int		fd;
  struct s_fd	*next;
}		t_fd;

char	*get_next_line(const int fd);

#endif /* !GET_NEXT_LINE */

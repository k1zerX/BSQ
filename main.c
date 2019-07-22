#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct    s_elem
{
  struct s_elem   *next;
  char            c;
}                 t_elem;

typedef struct    s_list
{
  t_elem          *start;
  size_t          len;
}                 t_list;

typedef struct    s_alphabet
{
  char            empty;
  char            obstacle;
  char            full;
}                 t_alphabet;

void    ft_exit()
{
  exit(1);
}

void    invalid()
{
  write(2, "map error\n", 10);
}

t_list  ft_readln(int fd)
{
  t_list  list;
  t_elem  *tmp;
  ssize_t  buf;

  if (!(tmp = malloc(sizeo(t_elem))))
    ft_exit();
  list->start = tmp;
  list->len = 1;
  while ((buf = read(fd, &(tmp->c), 1)) > 0 && tmp->c != '\n')
  {
    if (!(tmp->next = malloc(sizeof(t_elem))))
      ft_exit();
    ++list->len;
    tmp = tmp->next;
  }
  if (buf == -1)
    ft_exit();
  return(t_list)
}

int    init(char *arr, t_list list)
{
  t_elem  *tmp;
  t_elem  *buf;
  int     m;

  tmp = list.start;
  m = 0;
  while (--list.len > 3)
  {
    m *= 10;
    m += tmp->c - '0';
    buf = tmp;
    tmp = tmp->next;
    free(buf);
  }
  while (--list.len > 0)
  {
    *arr = tmp->c;
    ++arr;
    buf = tmp;
    tmp = tmp->next;
    free(buf);
  }
  free(tmp);
  return (m);
}

char    is_valid(char c, t_alphabet alph)
{
  return (c == alph.empty || c == alph.obstacle || c == alph.full);
}

int     first_read(int **arr, t_list list, t_alphabet alph, char **map)
{
  int     i;
  t_elem  *tmp;
  t_elem  *buf;
  
  if (!(*map = malloc(list.len * sizeof(char))))
    ft_exit();
  if (!(*arr = malloc((list.len - 1) * sizeof(char))))
    ft_exit();
  i = -1;
  while (++i < list.len - 1)
  {
    if (!is_valid(tmp->c))
      return (-1);
    (*map)[i] = tmp->c;
    arr[0][i] = tmp->c == alph.empty;
    buf = tmp;
    tmp = tmp->next;
    free(buf);
  }
  if (tmp->c != '\n')
    return (-1);
  (*map)[i] = tmp->c;
  free(tmp);
  return (list.len);
}

int     ft_min(int a, int b)
{
  return ((a < b) ? a : b);
}

void    print_map(char **map, int n, int m)
{
  int   i;

  i = -1;
  while (++i < m)
  {
    write(1, map[i], n);
    free(map[i]);
  }
  free(map);
}

char    bsq(int fd)
{
  t_alphabet  alph;
  int         n;
  int         m;
  int         *arr;
  int         i;
  int         j;
  char        **map;
  ssize_t     buf;

  m = init((char *)&alph, ft_readln(fd));
  if (m > 0 && !(map = malloc(m * sizeof(char *))))
    ft_exit();
  if ((n = first_read(&arr, ft_readln(fd), map, alph) < 2 || m < 1)
    return (0);
  i = 0;
  while (++i < m)
  {
    if (!(map[i] = malloc(n * sizeof(char))))
      ft_exit();
    if ((buf = read(fd, map[i], n)) == -1)
      ft_exit();
    if (buf != n || !is_valid(*map[i]))
      return (0);
    *arr = *map[i] == alph.empty;
    j = 0;
    while (++j < n - 1)
      if (!is_valid(map[i][j]))
        return (0);
      else
        arr[j] = (map[i][j] == alph.empty) ? (ft_min(arr[j], arr[j - 1]) + 1) : 0;
    if (map[i][j] != '\n')
      return (0);
  }
  print_map(map, n, m);
}

int     main(int ac, char *av[])
{
  int   fd;
  char  *file;
  
  file = av[1];
  if ((fd = (ac > 1) ? open(file, O_RDONLY) : 0) == -1)
    ft_exit();
  if (!bsq(fd))
    invalid();
  close(fd);
  while (--ac > 1)
  {
    write(1, "\n", 1);
    if ((fd = open(++file, O_RDONLY)) == -1)
      ft_exit();
    if (!bsq(fd))
      invalid();
    close(fd);
  }
  return 0;
}

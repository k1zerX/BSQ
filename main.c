#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

typedef struct		s_elem
{
	struct s_elem	*next;
	char			c;
}					t_elem;

typedef struct		s_list
{
	t_elem			*start;
	size_t			len;
}					t_list;

typedef struct		s_alph
{
	char			empty;
	char			obstacle;
	char			full;
}					t_alph;

typedef struct		s_map
{
	char			**map;
	int				m;
	int				n;
}					t_map;

typedef struct		s_sqr
{
	int				size;
	int				x;
	int				y;
}					t_sqr;

void		ft_exit()
{
	exit(1);
}

void		invalid()
{
	write(2, "map error\n", 10);
}

int			ft_min(int a, int b, int c)
{
	if (a < b)
	{
		if (a < c)
			return (a);
		else
			return (c);
	}
	else
	{
		if (b < c)
			return (b);
		else
			return (c);
	}
}

t_list		ft_readln(int fd)
{
	t_list		list;
	t_elem		*tmp;
	ssize_t		buf;

	if (!(tmp = malloc(sizeof(t_elem))))
		ft_exit();
	list.start = tmp;
	list.len = 1;
	while ((buf = read(fd, &(tmp->c), 1)) > 0 && tmp->c != '\n')
	{
		if (!(tmp->next = malloc(sizeof(t_elem))))
			ft_exit();
		++list.len;
		tmp = tmp->next;
	}
	if (buf == -1)
		ft_exit();
	return (list);
}

int			get_first_line(char *arr, t_list list)
{
	t_elem	*tmp;
	t_elem	*buf;
	int		m;

	tmp = list.start;
	m = 0;
	while (list.len > 4)
	{
		m *= 10;
		m += tmp->c - '0';
		buf = tmp;
		tmp = tmp->next;
		free(buf);
		--list.len;
	}
	while (list.len > 1)
	{
		*arr = tmp->c;
		++arr;
		buf = tmp;
		tmp = tmp->next;
		free(buf);
		--list.len;
	}
	free(tmp);
	return (m);
}

char		is_valid(char c, t_alph alph)
{
	return (c == alph.empty || c == alph.obstacle || c == alph.full);
}

void		print_map(t_map map)
{
	int		i;

	i = -1;
	while (++i < map.m)
	{
		write(1, map.map[i], map.n);
		free(map.map[i]);
	}
	free(map.map);

}

void		fill_map(char **map, t_sqr sqr, char c)
{
	int		i;
	int		j;

	i = -1;
	while (++i < sqr.size)
	{
		j = -1;
		while (++j < sqr.size)
			map[sqr.y - i][sqr.x - j] = c;
	}
}

char		**new_map(int m, int n)
{
	char	**map;
	int		i;

	if (!(map = malloc(m * sizeof(char *))))
		ft_exit();
	i = -1;
	while (++i < m)
		if (!(map[i] = malloc(n * sizeof(char))))
			ft_exit();
	return (map);
}

void		del_map(t_map map)
{
	int		i;

	i = 0;
	while (i < map.m)
	{
		free(map.map[i]);
		++i;
	}
	free(map.map);
}

t_map		init(int fd, t_sqr *sqr, t_alph *alph, int **arr)
{
	t_list	list;
	t_map	map;
	int		i;
	t_elem	*buf;
	t_elem	*tmp;

	map.m = get_first_line((char *)alph, ft_readln(fd));
	list = ft_readln(fd);
	map.map = new_map(map.m, (map.n = list.len));
	if (!(*arr = malloc((map.n - 1) * sizeof(int))))
		ft_exit();
	tmp = list.start;
	i = 0;
	while (i < list.len - 1)
	{
		if (!is_valid(((*map.map)[i] = tmp->c), *alph))
			goto INVALID_MAP;
		if (((*arr)[i] = (tmp->c == alph->empty)) > sqr->size)
		{
			sqr->size = (*arr)[i];
			sqr->x = i;
			sqr->y = 0;
		}
		buf = tmp;
		tmp = tmp->next;
		free(buf);
		++i;
	}
	if (((*map.map)[i] = tmp->c) != '\n')
		goto INVALID_MAP;
	free(tmp);
	return (map);
	INVALID_MAP:
	free(*arr);
	del_map(map);
	map.map = NULL;
	return (map);
}

char		bsq(int fd)
{
	t_map	map;
	t_sqr	sqr;
	t_alph	alph;
	int		*arr;
	ssize_t	buf;
	int		buf1;
	int		buf2;
	int		i;
	int		j;

	sqr.size = 0;
	map = init(fd, &sqr, &alph, &arr);
	if (!map.map)
		return (0);
	i = 1;
	while (i < map.m)
	{
		if ((buf = read(fd, map.map[i], map.n)) < 0)
			ft_exit();
		if (buf != map.n)
			goto INVALID_MAP;
		if (!is_valid(*map.map[i], alph))
			goto INVALID_MAP;
		if ((buf1 = (*map.map[i] == alph.empty)) > sqr.size)
		{
			sqr.size = *arr;
			sqr.x = 0;
			sqr.y = i;
		}
		j = 1;
		while (j < map.n - 1)
		{
			if (!is_valid(map.map[i][j], alph))
				goto INVALID_MAP;
			if ((buf2 = (map.map[i][j] == alph.empty) ? (ft_min(arr[j - 1], arr[j], buf1) + 1) : (0)) > sqr.size)
			{
				sqr.size = buf2;
				sqr.x = j;
				sqr.y = i;
			}
			arr[j - 1] = buf1;
			buf1 = buf2;
			++j;
		}
		if (map.map[i][j] != '\n')
			goto INVALID_MAP;
		arr[j - 1] = buf1;
		++i;
	}
	if ((buf = read(fd, (char *)&buf2, 1)) < 0)
		ft_exit();
	if (buf)
		goto INVALID_MAP;
	free(arr);
	fill_map(map.map, sqr, alph.full);
	print_map(map);
	return (1);
	INVALID_MAP:
	free(arr);
	del_map(map);
	return (0);
}

int			main(int ac, char *av[])
{
	int		fd;
	char	*file;

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
	return (0);
}

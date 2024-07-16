/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychng <ychng@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 03:43:34 by ychng             #+#    #+#             */
/*   Updated: 2024/06/25 15:00:07by ychng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "main.h"

# define ERROR -1
# define SUCCESS 0
# define WINDOW_WIDTH 1280
# define WINDOW_HEIGHT 720
# define TILE_SIZE 20
# define PLAYER_SIZE 3
# define PI 3.1415
# define LINE_LENGTH 20


typedef struct s_data
{
	char			*north_texture;
	char			*south_texture;
	char			*west_testure;
	char			*east_texture;
	unsigned short	floorcolor[3];
	unsigned short	ceilingcolor[3];
	bool			is_width_set;
	bool			is_height_set;
	bool			is_north_texture_set;
	bool			is_south_texture_set;
	bool			is_west_texture_set;
	bool			is_east_texture_set;
	bool			is_floorcolor_set;
	bool			is_ceilingcolor_set;
}	t_data;

typedef struct s_cast
{
	long	xintercept;
	long	yintercept;
	long	xstep;
	long	ystep;
}	t_cast;

typedef struct s_ray
{
	double	ray_angle;	
	bool	facing_up;
	bool	facing_down;
	bool	facing_right;
	bool	facing_left;
	t_cast	casts;
}	t_ray;

typedef struct s_raycast
{
	double	fov_angle;
	size_t	wall_strip_width;
	size_t	num_rays;
	t_ray	*rays;
}	t_raycast;

typedef struct s_player
{
	double	x;
	double	y;
	double	x_tile_pos;
	double	y_tile_pos;
	int		turn_direction;
	int		walk_direction;
	double	rotating_angle;
	double	move_speed;
	double	rotation_speed;
	size_t	keys[255];
	t_raycast	raycast;
}	t_player;

typedef struct s_map
{
	size_t		used_rows;
	size_t		alloc_rows;
	char		**map2d;
	t_player	player;
}	t_map;

typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_len;
	int		endian;
}	t_img;

typedef struct s_var
{
	void	*mlx;
	void	*win;
	t_img	data;
}	t_var;

typedef struct s_main
{
	t_data	data;
	t_map	map;
	t_var	var;
}	t_main;

int	error_with_message(char	*msg)
{
	ft_putstr_fd(msg, 2);
	ft_putchr_fd('\n', 2);
	return (ERROR);
}

void	exit_with_message(char *msg)
{
	ft_putstr_fd(msg, 2);
	ft_putchr_fd('\n', 2);
	exit(-1);
}

bool	extension_is_cub(char *mapname)
{
	size_t	count;
	char	*token;

	count = 0;
	token = ft_strdup(mapname);
	token = ft_strtok(token, ".");
	while (!(token == NULL))
	{
		if (ft_strcmp(token, "cub") == 0)
			count++;
		token = ft_strtok(NULL, ".");
	}
	free(token);
	return (count == 1);
}

bool	is_within_unsigned_short(char *value_str)
{
	int	value;

	value = ft_atoi(value_str);
	return (value >= 0 && value <= USHRT_MAX);
}

bool	is_valid_path(char *path)
{
	return (!ft_strncmp(path, "./", 2) && ft_isalnum(path[2]));
}

int	set_texture(char **texture, bool *is_set)
{
	char	*path;

	path = ft_strtok(NULL, " ");
	if (!path)
		return (error_with_message("error: missing path."));
	if (!is_valid_path(path))
		return (error_with_message("error: invalid path."));
	*texture = path;
	*is_set = true;
	return (SUCCESS);
}


int	validate_color_format(char *rgb)
{
	unsigned short	count;
	char			*token;

	count = 0;
	while (count < 3)
	{
		if (count == 0)
			token = ft_strtok(rgb, ",");
		else
			token = ft_strtok(NULL, ",");
		if (!(token && is_all_digits(token) && is_in_rgb_range(token)))
			return (ERROR);
		count++;
	}
	token = ft_strtok(NULL, ",");
	if (token)
		return (ERROR);
	return (SUCCESS);
}

int	set_color(unsigned short *color, bool *is_color_set)
{
	char	*rgb;
	char	*rgb_cpy;

	rgb = ft_strtok(NULL, " ");
	if (!rgb)
		return (error_with_message("error: missing either floor or ceiling color."));
	if (ft_strchr(rgb, '\n') && rgb[0] != '\n')
		rgb_cpy = ft_strtrim(rgb, "\n");
	if (validate_color_format(rgb_cpy) == ERROR)
		return (error_with_message("error: invalid color format."));
	free(rgb_cpy);
	color[0] = (unsigned short)ft_atoi(ft_strtok(rgb, ","));
	color[1] = (unsigned short)ft_atoi(ft_strtok(NULL, ","));
	color[2] = (unsigned short)ft_atoi(ft_strtok(NULL, ","));
	*is_color_set = true;
	return (SUCCESS);
}

int	set_identifiers_value(t_data *data, char *line)
{
	char	*token;

	token = ft_strtok(line, " ");
	if (token)
	{
		// if (ft_strcmp(token, "R"))
		// 	return (set_resolution(data));
		if (ft_strcmp(token, "NO") == 0)
			return (set_texture(&data->north_texture, &data->is_north_texture_set));
		else if (ft_strcmp(token, "SO") == 0)
			return (set_texture(&data->south_texture, &data->is_south_texture_set));
		else if (ft_strcmp(token, "WE") == 0)
			return (set_texture(&data->west_testure, &data->is_west_texture_set));
		else if (ft_strcmp(token, "EA") == 0)
			return (set_texture(&data->east_texture, &data->is_east_texture_set));
		else if (ft_strcmp(token, "F") == 0)
			return (set_color(data->floorcolor, &data->is_floorcolor_set));
		else if (ft_strcmp(token, "C") == 0)
			return (set_color(data->ceilingcolor, &data->is_ceilingcolor_set));
	}
	// free(token);
	return (SUCCESS);
}

bool	all_identifiers_set(t_data *data)
{
	return (data->is_north_texture_set && \
			data->is_south_texture_set && \
			data->is_west_texture_set && \
			data->is_east_texture_set && \
			data->is_floorcolor_set && \
			data->is_ceilingcolor_set);
}

bool	can_parse_mapdata(t_main *main, int fd)
{
	char	*line;

	while (true)
	{
		if (all_identifiers_set(&main->data))
			break ;
		line = get_next_line(fd);
		if (!line)
			break ;
		else if (set_identifiers_value(&main->data, line) == ERROR)
		{
			free(line);
			return (false);
		}
		free(line);
	}
	return (all_identifiers_set(&main->data));
}


bool	has_valid_characters(char *line)
{
	char	*trimmed_line;
	size_t	trim_len;
	size_t	span_len;

	trimmed_line = ft_strtrim(line, " ");
	trim_len = ft_strlen(trimmed_line);
	span_len = ft_strspn(line, "01NSEW");
	free(trimmed_line);
	return (trim_len == span_len);
}







int	validate_top_to_bottom(t_map *map)
{
	size_t	col;
	size_t	row;
	char	curr_col;
	char	top_col;
	char	bottom_col;

	col = 0;
	while (col < get_longest_row_len(map))
	{
		row = 1;
		while (row < (map->used_rows - 1))
		{
			curr_col = map->map2d[row][col];
			top_col = map->map2d[row - 1][col];
			bottom_col = map->map2d[row + 1][col];
			if (ft_strchr("0NSEW", curr_col) && !(top_col != ' ' && bottom_col != ' '))
				return (error_with_message("error: there are top / bottom gap."));
			row++;
		}
		col++;
	}
	return (SUCCESS);
}

int	validate_left_to_right(t_map *map)
{
	size_t	row;
	size_t	col;
	char	curr_col;
	char	left_col;
	char	right_col;

	row = 0;
	while (row < map->used_rows)
	{
		col = 1;
		while (col < (get_longest_row_len(map) - 1))
		{
			curr_col = map->map2d[row][col];
			left_col = map->map2d[row][col - 1];
			right_col = map->map2d[row][col + 1];
			if (ft_strchr("0NSEW", curr_col) && !(left_col != ' ' && right_col != ' '))
				return (error_with_message("error: there are left / right gap."));
			col++;
		}
		row++;
	}
	return (SUCCESS);
}



bool	pad_2d_map(t_map *map)
{
	size_t	max_col;
	size_t	i;
	size_t	curr_row_len;

	max_col = get_longest_row_len(map);
	i = 0;
	while (i < map->used_rows)
	{
		curr_row_len = ft_strlen(map->map2d[i]);
		if (curr_row_len < max_col)
		{
			if (pad_row(&map->map2d[i], max_col) == ERROR)
				return (false);
		}
		i++;
	}
	return (true);
}

void	set_player_pos(t_main *main)
{
	t_player	*player;
	size_t		i;
	size_t		j;
	char		curr_col;

	player = &main->map.player;
	i = 0;
	while (i < main->map.used_rows)
	{
		j = 0;
		while (j < (size_t)ft_strlen(main->map.map2d[i]))
		{
			curr_col = main->map.map2d[i][j];
			if (ft_strchr("NSEW", curr_col))
			{
				player->x = j;
				player->y = i;
			}
			j++;
		}
		i++;
	}
}


void	parse_map(t_main *main, char *mapname)
{
	int	fd;

	if (!extension_is_cub(mapname))
		exit_with_message("error: invalid map extension.");
	fd = open(mapname, O_RDONLY);
	if (fd < 0)
		exit_with_message("error: opening map.");
	if (!can_parse_mapdata(main, fd))
	{
		close(fd);
		exit_with_message("error: parsing map data.");
	}
	if (!can_parse_mapcontent(main, fd))
	{
		close(fd);
		exit_with_message("error: parsing map content.");
	}
	close(fd);
}

void	initialize_data(t_data *data)
{
	data->north_texture = NULL;
	data->south_texture = NULL;
	data->west_testure = NULL;
	data->east_texture = NULL;
	data->floorcolor[0] = 0;
	data->floorcolor[1] = 0;
	data->floorcolor[2] = 0;
	data->ceilingcolor[0] = 0;
	data->ceilingcolor[1] = 0;
	data->ceilingcolor[2] = 0;
	data->is_width_set = false;
	data->is_height_set = false;
	data->is_north_texture_set = false;
	data->is_south_texture_set = false;
	data->is_west_texture_set = false;
	data->is_east_texture_set = false;
	data->is_floorcolor_set = false;
	data->is_ceilingcolor_set = false;
}

void	initialize_ray(t_raycast *raycast)
{
	raycast->fov_angle = 60 * (PI / 180);
	raycast->wall_strip_width = 50;
	raycast->num_rays = WINDOW_WIDTH / raycast->wall_strip_width;
	raycast->rays = ft_calloc(sizeof(t_raycast), raycast->num_rays);
}

void	initialize_player(t_player *player)
{
	ft_memset(player->keys, 0, sizeof(player->keys));
	player->x = -1;
	player->y = -1;
	player->x_tile_pos = -1;
	player->y_tile_pos = -1;
	player->walk_direction = 0;
	player->turn_direction = 0;
	player->turn_direction = 0;
	player->rotating_angle = PI / 2;
	player->move_speed = 0.005f;
	player->rotation_speed = 0.25 * (PI / 180);
	initialize_ray(&player->raycast);
}

void	initialize_map(t_map *map)
{
	map->used_rows = 0;
	map->alloc_rows = 0;
	map->map2d = NULL;
	initialize_player(&map->player);
}

void	initialize_img(t_var *var)
{
	t_img	*data;

	data = &var->data;
	data->img = mlx_new_image(var->mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
	data->addr = mlx_get_data_addr(data->img, &data->bits_per_pixel, &data->line_len, &data->endian);
}

void	initialize_var(t_var *var)
{
	var->mlx = mlx_init();
	var->win = mlx_new_window(var->mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "Cub3D");
	initialize_img(var);
}


void	initialize_main(t_main *main)
{
	initialize_data(&main->data);
	initialize_map(&main->map);
	initialize_var(&main->var);
}

void	my_mlx_pixel_put(t_img *data, size_t x, size_t y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_len + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

// Bitshifting color
int	create_trgb(int t, int r, int g, int b)
{
	return (t << 24 | r << 16 | g << 8 | b);
}

int	get_t(int trgb)
{
	return ((trgb >> 24) & 0xFF);
}

int	get_r(int trgb)
{
	return ((trgb >> 16) & 0xFF);
}

int	get_g(int trgb)
{
	return ((trgb >> 8) & 0xFF);
}

int	get_b(int trgb)
{
	return (trgb & 0xFF);
}

void	draw_tile(t_img *data, size_t x, size_t y, int color)
{
	size_t	start_x;
	size_t	start_y;
	size_t	x_span;
	size_t	y_span;

	start_x = x * TILE_SIZE;
	start_y = y * TILE_SIZE;
	x_span = 0;
	while (x_span < TILE_SIZE)
	{
		y_span = 0;
		while (y_span < TILE_SIZE)
		{
			my_mlx_pixel_put(data, start_x + x_span, start_y + y_span , color);
			y_span++;
		}
		x_span++;
	}
}

void	draw_minimap(t_main *main)
{
	size_t	x;
	size_t	y;

	x = 0;
	while (x < main->map.used_rows)
	{
		y = 0;
		while (y < (size_t)ft_strlen(main->map.map2d[x]))
		{
			if (main->map.map2d[x][y] == '1')
				draw_tile(&main->var.data, y, x, create_trgb(1, 255, 255, 255));
			else if (ft_strchr("0N", main->map.map2d[x][y]))
				draw_tile(&main->var.data, y, x, create_trgb(1, 0, 0, 255));
			y++;
		}
		x++;
	}
}

int ft_round(float num)
{
    return (int)(num + 0.5);
}

int ft_abs(int num)
{
	if (num < 0)
		return (-num);
    return (num);
}


void	draw_line(t_main *main, int x0, int y0, int x1, int y1)
{
	int	delta_x;
	int	delta_y;
	int	side_len;
	float	x_inc;
	float	y_inc;
	float	current_x;
	float	current_y;
	int		i;

	delta_x = (x1 - x0);
	delta_y = (y1 - y0);
	if (ft_abs(delta_x) >= ft_abs(delta_y))
		side_len = ft_abs(delta_x);
	else
		side_len = ft_abs(delta_y);
	x_inc = delta_x / (float)side_len;
	y_inc = delta_y / (float)side_len;
	current_x = x0;
	current_y = y0;
	i = 0;
	while (i <= side_len)
	{
		my_mlx_pixel_put(&main->var.data, ft_round(current_x), ft_round(current_y), create_trgb(1, 255, 0, 0));
		current_x += x_inc;
		current_y += y_inc;
		i++;
	}
}

void	draw_angle_line(t_main *main)
{
	int	start_x;
	int	start_y;
	int	end_x;
	int	end_y;

	start_x = main->map.player.x * TILE_SIZE;
	start_y = main->map.player.y * TILE_SIZE;
	start_x = start_x + (TILE_SIZE) / 2;
	start_y = start_y + (TILE_SIZE) / 2;
	end_x = start_x + cos(main->map.player.rotating_angle) * LINE_LENGTH;
	end_y = start_y + sin(main->map.player.rotating_angle) * LINE_LENGTH;
	draw_line(main, start_x, start_y, end_x, end_y);
}

void	draw_player_dot(t_main *main)
{
	double	start_x;
	double	start_y;
	double	box_offset_x;
	double	box_offset_y;
	size_t	x_span;
	size_t	y_span;

	start_x = (float)main->map.player.x * TILE_SIZE;
	start_y = (float)main->map.player.y * TILE_SIZE;
	box_offset_x = ceil((TILE_SIZE - PLAYER_SIZE) / (float)2);
	box_offset_y = ceil((TILE_SIZE - PLAYER_SIZE) / (float)2);
	// printf("%f\n", box_offset_x);
	x_span = 0;
	while (x_span < PLAYER_SIZE)
	{
		y_span = 0;
		while (y_span < PLAYER_SIZE)
		{
			my_mlx_pixel_put(&main->var.data, start_x + box_offset_x + x_span, start_y + box_offset_y + y_span, create_trgb(1, 255, 0, 0));
			main->map.player.x_tile_pos  = start_x + box_offset_x + x_span;
			main->map.player.y_tile_pos  = start_y + box_offset_y + y_span;
			y_span++;
		}
		x_span++;
	}
	draw_angle_line(main);
	// draw_line(main, start_x, start_y, start_x + (TILE_SIZE * 2), start_y + (TILE_SIZE * 2));
}

// void	move_player(t_main *main, int dx, int dy)
// {
// 	int	new_x;
// 	int	new_y;

// 	new_x = main->map.player.x + dx;
// 	new_y = main->map.player.y + dy;
// 	if (new_x >= 0 && new_x < ft_strlen(main->map.map2d[new_y]) && new_y >= 0 && new_y < main->map.used_rows && main->map.map2d[new_y][new_x] != '1')
// 	{
// 		main->map.map2d[main->map.player.y][main->map.player.x] = '0';
// 		main->map.map2d[new_y][new_x] = 'N';
// 		main->map.player.x = new_x;
// 		main->map.player.y = new_y;
// 	}
// }

int	key_press(int keycode, t_main *main)
{
	if (keycode < 256)
		main->map.player.keys[keycode] = 1;
	else if (keycode == 65307)
	{
		mlx_destroy_window(main->var.mlx, main->var.win);
		exit(0);
	}
	return (SUCCESS);
}

int	key_release(int keycode, t_main *main)
{
	if (keycode < 256)
		main->map.player.keys[keycode] = 0;
	return (SUCCESS);
}

void	process_movement(t_main *main)
{
	if (main->map.player.keys[119]) // W
		main->map.player.walk_direction = 1;
	else if (main->map.player.keys[115]) // D
		main->map.player.walk_direction = -1;
	else
		main->map.player.walk_direction = 0;

	if (main->map.player.keys[97]) // A
		main->map.player.turn_direction = 1;
	else if (main->map.player.keys[100]) // S
		main->map.player.turn_direction = -1;
	else
		main->map.player.turn_direction = 0;

	if (main->map.player.turn_direction)
		main->map.player.rotating_angle += main->map.player.turn_direction * main->map.player.rotation_speed;
	if (main->map.player.walk_direction)
	{
		double	move_step;
	
		move_step = main->map.player.walk_direction * main->map.player.move_speed;
		double	new_x;
		double	new_y;
		new_x = main->map.player.x + cos(main->map.player.rotating_angle) * move_step;
		new_y = main->map.player.y + sin(main->map.player.rotating_angle) * move_step;
		if (main->map.map2d[ft_round(new_y)][ft_round(new_x)] != '1')
		{
			main->map.player.x = new_x ;
			main->map.player.y = new_y ;
		}
	}
}



double	normalize_angle(double angle)
{
	double	normalize;

	normalize = fmod(angle, 2 * PI);
	if (angle < 0)
		normalize += 2 * PI;
	return (normalize);
}

void	cast(t_main *main, size_t i)
{
	t_player	*player;
	size_t	xintercept;
	size_t	yintercept;
	size_t	xstep;
	size_t	ystep;

	player = &main->map.player;
	//////////////////////////////////
	// horizontal ray grid intersection code
	//////////////////////////////////
	bool	horizontal_wall_hit;
	size_t	wall_hit_x;
	size_t	wall_hit_y;


	horizontal_wall_hit = false;
	wall_hit_x = 0;
	wall_hit_y = 0;
	yintercept = floor(player->y_tile_pos / TILE_SIZE) * TILE_SIZE;
	if (player->raycast.rays[i].facing_down)
		yintercept += TILE_SIZE;
	xintercept = player->y_tile_pos + (yintercept - player->y_tile_pos) / tan(player->raycast.rays[i].ray_angle);

	// calculate the increment xstep and ystep
	ystep = TILE_SIZE;
	if (player->raycast.rays[i].facing_up)
		ystep *= -1;
	xstep = TILE_SIZE / tan(player->raycast.rays[i].ray_angle);
	if (player->raycast.rays[i].facing_left && xstep > 0)
		xstep *= -1;
	else if (player->raycast.rays[i].facing_right && xstep < 0)
		xstep *= -1;
	
	long	next_horizontal_touch_x;
	long	next_horizontal_touch_y;

	next_horizontal_touch_x = xintercept;
	next_horizontal_touch_y = yintercept;

	if (player->raycast.rays[i].facing_up)
		next_horizontal_touch_y--;
	while (next_horizontal_touch_x >= 0 && next_horizontal_touch_x <= WINDOW_WIDTH && next_horizontal_touch_y >= 0 && next_horizontal_touch_y <= WINDOW_HEIGHT)
	{
		if (main->map.map2d[next_horizontal_touch_x / TILE_SIZE][next_horizontal_touch_y / TILE_SIZE] == '1')
		{
			horizontal_wall_hit = true;
			wall_hit_x = next_horizontal_touch_x;
			wall_hit_y = next_horizontal_touch_y;
			// draw_line(main, player->x * TILE_SIZE, player->y * TILE_SIZE, wall_hit_x, wall_hit_y);
			break ;
		}
		else
		{
			next_horizontal_touch_x += xstep;
			next_horizontal_touch_y += ystep;
		}
	}
}

bool	is_ray_facing_down(double ray_angle)
{
	return (ray_angle > 0 && ray_angle < PI);
}

bool	is_ray_facing_right(double ray_angle)
{
	return (ray_angle < 0.5 * PI || ray_angle > 1.5 * PI);
}

void	cast_all_rays(t_main *main)
{
	t_player	*player;
	size_t	column_id;
	double	ray_angle;
	size_t	i;

	player = &main->map.player;
	column_id;
	ray_angle = normalize_angle(player->rotating_angle - (player->raycast.fov_angle / 2));
	i = 0;
	while (i < 1)
	{
		player->raycast.rays[i].ray_angle = ray_angle;
		player->raycast.rays[i].facing_down = is_ray_facing_down(ray_angle);
		player->raycast.rays[i].facing_up = !is_ray_facing_down(ray_angle);
		player->raycast.rays[i].facing_right = is_ray_facing_right(ray_angle);
		player->raycast.rays[i].facing_left = !is_ray_facing_right(ray_angle);
		ray_angle += normalize_angle(player->raycast.fov_angle / player->raycast.num_rays);
		cast(main, i);
		column_id++;
		i++;
	}
}

void	draw_rays(t_main *main)
{
	int	start_x;
	int	start_y;
	int	end_x;
	int	end_y;
	int	i;

	start_x = main->map.player.x * TILE_SIZE;
	start_y = main->map.player.y * TILE_SIZE;
	start_x = start_x + (TILE_SIZE) / 2;
	start_y = start_y + (TILE_SIZE) / 2;
	i = 0;
	// while (i < main->map.player.raycast.num_rays)
	while (i < 1)
	{
		end_x = start_x + cos(main->map.player.raycast.rays[i].ray_angle) * 50;
		end_y = start_y + sin(main->map.player.raycast.rays[i].ray_angle) * 50;
		draw_line(main, start_x, start_y, end_x, end_y);
		i++;
	}
}

int	render_next_frame(void *s_main)
{
	t_main	*main;
	t_var	*var;

	main = (t_main *)s_main;
	var = &main->var;
	process_movement(main);
	draw_minimap(main);
	cast_all_rays(main);
	draw_rays(main);
	draw_player_dot(main);
	mlx_put_image_to_window(var->mlx, var->win, var->data.img, 0, 0);
	return (SUCCESS);
}

int main(int argc, char **argv)
{
	t_main	main;

	if (!(argc == 2))
		return (error_with_message("error: too many arguments."));
	initialize_main(&main);
	parse_map(&main, argv[1]);
	mlx_hook(main.var.win, 2, 1L<<0, key_press, &main);
	mlx_hook(main.var.win, 3, 1L<<1, key_release, &main);
	mlx_loop_hook(main.var.mlx, render_next_frame, &main);
	mlx_loop(main.var.mlx);
}

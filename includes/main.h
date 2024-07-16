/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychng <ychng@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 20:11:02 by ychng             #+#    #+#             */
/*   Updated: 2024/07/17 02:05:05 by ychng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include "system.h"
# include "libft.h"
# include "mlx.h"

# define PASS true
# define FAIL false

# define W_WIDTH 1260
# define W_HEIGHT 720

# define M_WALL '1'
# define M_PATH '0'
# define M_NORTH 'N'

# define TILE_SIZE 30
# define PLAYER_SIZE 5
# define LINE_LENGTH 15

# define PI 3.14159265358979323846264338327950288420

typedef struct	s_mapinfo
{
	char	*north_texture;
	char	*south_texture;
	char	*east_texture;
	char	*west_testure;
	int		floorcolor[3];
	int		ceilingcolor[3];
	bool	is_north_texture_set;
	bool	is_south_texture_set;
	bool	is_east_texture_set;
	bool	is_west_texture_set;
	bool	is_floorcolor_set;
	bool	is_ceilingcolor_set;
}	t_mapinfo;

typedef struct	s_mapdata
{
	int		rows;
	int		cols;
	char	**map2d;
}	t_mapdata;

typedef struct	s_image
{
	void	*img;
	char	*addr;
	int		bpp;
	int		stride;
	int		endian;		
}	t_image;

typedef struct	s_minilibx
{
	void	*mlx;
	void	*win;
	t_image	image;
}	t_minilibx;

typedef struct	s_player
{
	int		center_x;
	int		center_y;
	int		topleft_x;
	int		topleft_y;
	double	rotation_angle;
	double	rotation_speed;
}	t_player;

typedef struct	s_main
{
	t_mapinfo	mapinfo;
	t_mapdata	mapdata;
	t_minilibx	minilibx;
	t_player	player;
}	t_main;

typedef struct	s_points
{
	int	x0;
	int	y0;
	int	x1;
	int	y1;
}	t_points;

typedef struct	s_dda
{
	int			delta_x;
	int			delta_y;
	int			side_len;
	double		x_inc;
	double		y_inc;
	double		curr_x;
	double		curr_y;
}	t_dda;


// get_nextline/
bool	contains_newline(char *remaining_line);
int		read_and_check(int fd, char *buffer, int buffer_size);
char	*get_next_line(int fd);

// error_output/
void	exit_with_error(char *msg);
bool	return_with_error(char *msg);

// initialize_struct/
void	init_minilibx(t_minilibx *minilibx);
void	init_main(t_main *main);

// parse_map/
bool	extension_is_cub(char *mapname);
bool	set_direction_texture(t_mapinfo *mapinfo, char *token);
bool	set_color(t_mapinfo *mapinfo, char *token);
bool	meets_required_info(t_mapinfo *mapinfo, int fd);
int		get_max_cols(t_mapdata *mapdata);
bool	pad_2d_map(t_mapdata *mapdata);
bool	parse_data(t_main *main, int fd);
bool	validate_top_and_bottom_border(t_mapdata *mapdata);
bool	validate_left_and_right_border(t_mapdata *mapdata);
bool	validate_rows(t_mapdata *mapdata);
bool	validate_cols(t_mapdata *mapdata);
bool	validate_2d_map(t_mapdata *mapdata);
bool	meets_required_data(t_main *main, int fd);
void	parse_map(t_main *main, char *mapname);

// render_frame/
int		rgb(int r, int g, int b);
void	pixel_put(t_image *image, int x, int y, int color);
void	dda(t_main *main, t_points *points);
void	draw_map(t_main *main);
void	draw_player(t_main *main);
int		render_frame(void *main);

#endif

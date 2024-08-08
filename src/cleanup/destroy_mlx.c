/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   destroy_mlx.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ghwa <ghwa@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 00:12:30 by ychng             #+#    #+#             */
/*   Updated: 2024/08/08 12:51:08 by ghwa             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	destroy_mlx(t_main *main)
{
	t_minilibx	*minilibx;
	int			i;

	minilibx = &main->minilibx;
	mlx_destroy_image(minilibx->mlx, minilibx->image.img);
	i = -1;
	while (++i < 4)
		mlx_destroy_image(minilibx->mlx, minilibx->texture[i].img);
	mlx_destroy_window(minilibx->mlx, main->minilibx.win);
	// mlx_destroy_display(minilibx->mlx);
	free(minilibx->mlx);
}

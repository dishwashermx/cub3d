/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_rays_angle.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychng <ychng@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 09:53:46 by ychng             #+#    #+#             */
/*   Updated: 2024/08/08 13:34:55 by ychng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	set_rays_angle(t_main *main)
{
	int		columnid;
	t_ray	*ray;
	double	ray_angle;
	double	dist_proj_plane;

	columnid = 0;
	dist_proj_plane = (main->mapdata.m_width / 2) / tan(main->raycast.fov_angle / 2);
	while (columnid < main->raycast.num_of_rays)
	{
		ray = &main->raycast.rays[columnid];
		ray->columnid = columnid;
		ray->ray_angle = normalized_angle(main->player.rotation_angle + atan((columnid - (main->raycast.num_of_rays/2)) / dist_proj_plane));
		ray->is_facing_down = is_ray_facing_down(ray->ray_angle);
		ray->is_facing_up = !ray->is_facing_down;
		ray->is_facing_right = is_ray_facing_right(ray->ray_angle);
		ray->is_facing_left = !ray->is_facing_right;
		columnid++;
	}
}

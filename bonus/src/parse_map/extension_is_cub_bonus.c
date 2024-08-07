/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extension_is_cub_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ychng <ychng@student.42kl.edu.my>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 02:13:05 by ychng             #+#    #+#             */
/*   Updated: 2024/08/07 17:29:32 by ychng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main_bonus.h"

bool	extension_is_cub(char *mapname)
{
	int		count;
	char	*mapname_dup;
	char	*token;

	count = 0;
	mapname_dup = ft_strdup(mapname);
	token = ft_strtok(mapname_dup, ".");
	while (!(token == NULL))
	{
		if (ft_strcmp(token, "cub") == 0)
			count++;
		token = ft_strtok(NULL, ".");
	}
	free(mapname_dup);
	return (count == 1);
}

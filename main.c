/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:18:27 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/20 18:25:31 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "phulo.h"

void	data_input(int argc, char *argv[], t_data data)
{
	data->N = ft_atoi(argv[1]);
	data->n = ft_atoi(argv[1]);
	data->t_die = ft_atoi(argv[2]);
	data->t_eat = ft_atoi(argv[3]);
	data->t_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->m_eat = ft_atoi(argv[5]);
}

int	main(int argc, char *argv[])
{
	t_data data;
	if (argc < 5 || argc > 6)
	{
		printf("wrong quantity of arguments\n");
		return (0);
	}
	data_input(argc, argv, data);
	return (0);
}

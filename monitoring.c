/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 17:18:46 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/23 17:18:47 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor(void	*arg)
{
	t_data	*data = (t_data *)arg;
	t_philo	*p;
	int		i;
	long long	time_since_meal;
	long long	ts;

	while (1)
	{
		i = 0;
		while (i < data->n_philo)
		{
			p = &data->philos[i];
			pthread_mutex_lock(&data->death_mutex);
			time_since_meal = get_timestamp() - p->last_meal_time;
			if (time_since_meal > data->t_die && p->is_eating == 0) 
			{
				data->simulation_stop = 1;
				pthread_mutex_lock(&data->write_mutex);
				ts = get_timestamp() - data->start_time;
				printf("%lld %d died\n", ts, p->id + 1);
				pthread_mutex_unlock(&data->write_mutex);
				pthread_mutex_unlock(&data->death_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&data->death_mutex);
			i++;
		}
		usleep(500);
	}
}
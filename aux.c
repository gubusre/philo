/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aux.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 15:40:21 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/23 15:40:23 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + tv.tv_usec / 1000);
}

void	cleanup(t_data *data)
{
	int	i;

	i = 0;
	while(i < data->n_philo)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data->write_mutex);
	pthread_mutex_destroy(&data->death_mutex);
	pthread_mutex_destroy(&data->full_mutex);
	free(data->forks);
	free(data->philos);
}

void	smart_sleep(long long duration, t_data *data)
{
	long long	start;

	start = get_timestamp();
	while (!simulation_should_stop(data))
	{
		if (get_timestamp() - start >= duration)
			break ;
		usleep(500);
	}
}

void print_action(t_philo *p, const char *msg)
{
	long long	timestamp;

	pthread_mutex_lock(&p->data->write_mutex);
	if (simulation_should_stop(p->data))
	{
		pthread_mutex_unlock(&p->data->write_mutex);
		return ;
	}
	timestamp = get_timestamp() - p->data->start_time;
	printf("%lld %d %s\n", timestamp, p->id + 1, msg);
	pthread_mutex_unlock(&p->data->write_mutex);
}

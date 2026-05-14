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

#include "philo.h"

void	join_threads(t_data *data)
{
	int i;

	i = 0;
	while(i < data->n_philo)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
}

int	init_simulation(t_data *data)
{
	int i;
	pthread_t   monitor_thread;
	t_philo *p;

	i = 0;
	while (i < data->n_philo)
	{
		p = &data->philos[i];
		if (pthread_create(&p->thread, NULL, philo_routine, p) != 0)
			return (1);
		usleep(100);
		i++;
	}
	if (pthread_create(&monitor_thread, NULL, monitor, data) != 0)
                return (1);
        pthread_detach(monitor_thread);
	return (0);
}

int	init_philos(t_data *data)
{
	int	i;

	data->philos = malloc(sizeof(t_philo) * data->n_philo);
	if (!data->philos)
		return (1);
	i = 0;
	while (i < data->n_philo)
	{
		t_philo *p = &data->philos[i];
		p->id = i;
		p->data = data;
		p->left_fork = &data->forks[i];
		p->right_fork = &data->forks[(i + 1) % data->n_philo];
		p->last_meal_time = data->start_time;
		p->meals_eaten = 0;
		p->is_eating = 0;
		i++;
	}
	return (0);
}

int	init_data(t_data *data, int argc, char *argv[])
{
	int i;

	data->n_philo = atoi(argv[1]);
	data->t_die = atoi(argv[2]);
	data->t_eat = atoi(argv[3]);
	data->t_sleep = atoi(argv[4]);
	if (argc == 6)
		data->must_eat = atoi(argv[5]);
	else
		data->must_eat = -1;
	data->simulation_stop  = 0;
	data->philos_full = 0;
	pthread_mutex_init(&data->write_mutex, NULL);
	pthread_mutex_init(&data->death_mutex, NULL);
	pthread_mutex_init(&data->full_mutex, NULL);
	data->forks = malloc(sizeof(pthread_mutex_t) * data-> n_philo);
	if (!data->forks)
		return (1);
	i = 0;
	while (i < data->n_philo)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
	data->start_time = get_timestamp();
	return (0);
}

int	main(int argc, char *argv[])
{
	t_data data;
	t_philo	*philo;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: ./philo n_philo t_die t_eat t_sleep [must_eat]\n");
		return (0);
	}
	if (init_data(&data, argc, argv) != 0)
		return (1);
	if (init_philos(&data) != 0)
		return (1);
	if (init_simulation(&data) != 0)
	{
		cleanup(&data);
		return (1);
	}
	join_threads(&data);
	cleanup(&data);
	return (0);
}

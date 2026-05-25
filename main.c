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

int init_simulation(t_data *data)
{
    int i;
    pthread_t   monitor_thread;
    t_philo *p;

    data->start_time = get_timestamp();

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

int init_philos(t_data *data)
{
    int     i;

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
        p->left_index = i;
        p->right_index = (i + 1) % data->n_philo;
        p->is_full = 0;

        pthread_mutex_lock(&p->data->death_mutex);
        p->last_meal_time = get_timestamp();
        p->is_eating = 0;
        pthread_mutex_unlock(&p->data->death_mutex);

        p->meals_eaten = 0;
        i++;
    }
    return (0);
}

static int  is_valid_number(const char *str)
{
    int i;

    i = 0;
    if (!str || str[0] == '\0')
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

int init_data(t_data *data, int argc, char *argv[])
{
    int i;

    i = 1;
    while (i < argc)
    {
        if (!is_valid_number(argv[i]))
        {
            printf("Error: invalid argument '%s'\n", argv[i]);
            return (1);
        }
        i++;
    }

    data->n_philo = atoi(argv[1]);
    data->t_die   = atol(argv[2]);
    data->t_eat   = atol(argv[3]);
    data->t_sleep = atol(argv[4]);

    if (argc == 6)
        data->must_eat = atoi(argv[5]);
    else
        data->must_eat = -1;

    if (data->n_philo < 1 || data->t_die < 1 ||
        data->t_eat < 1 || data->t_sleep < 1)
        return (1);

    data->simulation_stop = 0;
    data->philos_full = 0;

    if (pthread_mutex_init(&data->write_mutex, NULL) != 0) return (1);
    if (pthread_mutex_init(&data->death_mutex, NULL) != 0) return (1);
    if (pthread_mutex_init(&data->full_mutex, NULL) != 0) return (1);

    data->forks = malloc(sizeof(pthread_mutex_t) * data->n_philo);
    if (!data->forks)
        return (1);

    for (int i = 0; i < data->n_philo; i++)
        pthread_mutex_init(&data->forks[i], NULL);
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

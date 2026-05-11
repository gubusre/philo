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

int init_simulation(t_data *data)
{
    pthread_t   monitor;
    t_philo     *p;
    int         i;

    i = -1;
    while (i++ < data->n_philo;)
    {
        p = &data->philos[i];
        if (pthread_create(&p->thread, NULL, philo_routine, p) != 0)
            return (1);

        usleep(100);
    }

    return (0);
}

int init_philos(t_data *data)
{
    data->philos = malloc(sizeof(t_philo) * data->n_philo);
    if (!data->philos)
        return (1);
    for (int i = 0; i < data->n_philo; i++)
    {
        t_philo *p = &data->philos[i];

        p->id = i;
        p->data = data;
        /*        ASIGNACIÓN DE FORKS       */
        p->left_fork  = &data->forks[i];
        p->right_fork = &data->forks[(i + 1) % data->n_philo];
        /*       ESTADO INICIAL DEL PHILO   */
        p->last_meal_time = data->start_time;
        p->meals_eaten = 0;
        p->is_eating = 0;
    }
    return (0);
}


int init_data(t_data *data, int argc, char **argv)
{
    data->n_philo = atoi(argv[1]);
    data->t_die   = atoll(argv[2]);
    data->t_eat   = atoll(argv[3]);
    data->t_sleep = atoll(argv[4]);
    if (argc == 6)
        data->must_eat = atoi(argv[5]);
    else
        data->must_eat = -1;
    data->simulation_stop = 0;
    data->philos_full = 0;
	/*         INIT GLOBAL MUTEX        */
    pthread_mutex_init(&data->write_mutex, NULL);
    pthread_mutex_init(&data->death_mutex, NULL);
    pthread_mutex_init(&data->full_mutex, NULL);
	/*         INIT FORKS ARRAY         */
    data->forks = malloc(sizeof(pthread_mutex_t) * data->n_philo);
    if (!data->forks)
        return (1);
    for (int i = 0; i < data->n_philo; i++)
        pthread_mutex_init(&data->forks[i], NULL);
	/*         INIT START TIME          */
    data->start_time = get_timestamp();
    return
}

int	main(int argc, char *argv[])
{
	t_data data;
	if (argc < 5 || argc > 6)
	{
		printf("wrong quantity of arguments\n");
		return (0);
	}
	init_data(&data, argc, argv);
	init_philos(&data);
	init_simulation(&data);

	return (0);
}

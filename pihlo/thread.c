/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:07:55 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/20 18:08:33 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int simulation_should_stop(t_data *data)
{
    int stop = 0;

    pthread_mutex_lock(&data->death_mutex);
    if (data->simulation_stop == 1)
        stop = 1;
    pthread_mutex_unlock(&data->death_mutex);
    if (stop)
        return (1);
    pthread_mutex_lock(&data->full_mutex);
    if (data->must_eat > 0 && data->philos_full == data->n_philo)
        stop = 1;
    pthread_mutex_unlock(&data->full_mutex);
    return (stop);
}

void *philo_routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    while (!simulation_should_stop(p->data))
    {
        take_forks(p);
        eat(p);
        release_forks(p);
        sleep_philo(p);
        think(p);
    }

    return (NULL);
}
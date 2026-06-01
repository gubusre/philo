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

void cleanup(t_data *data)
{
    int i;

    if (!data)
        return;
    i = 0;
    if (data->forks)
    {
        while(i < data->n_philo)
        {
            pthread_mutex_destroy(&data->forks[i]);
            i++;
        }
        free(data->forks);
    }
    if (data->philos)
    {
        i = 0;
        while (i < data->n_philo)
        {
            pthread_mutex_destroy(&data->philos[i].meal_mutex);
            i++;
        }
        free(data->philos);
    }
    pthread_mutex_destroy(&data->write_mutex);
    pthread_mutex_destroy(&data->death_mutex);
    pthread_mutex_destroy(&data->full_mutex);
}


void smart_sleep(long long duration, t_data *data)
{
    long long start = get_timestamp();
    while (!simulation_should_stop(data))
    {
        if (get_timestamp() - start >= duration)
            break;
        usleep(1000); /* 1 ms */
    }
}



void print_action(t_philo *p, const char *msg)
{
    if (simulation_should_stop(p->data))
        return;

    pthread_mutex_lock(&p->data->write_mutex);
    printf("%lld %d %s\n", get_timestamp() - p->data->start_time, p->id + 1, msg);
    pthread_mutex_unlock(&p->data->write_mutex);
}


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

void *monitor(void *arg)
{
    t_data *data = (t_data *)arg;
    long long time_since_meal;
    int i;

    while (1)
    {
        i = 0;
        while (i < data->n_philo)
        {
            pthread_mutex_lock(&data->death_mutex);
            time_since_meal = get_timestamp() - data->philos[i].last_meal_time;
            int eating = data->philos[i].is_eating;
            pthread_mutex_unlock(&data->death_mutex);

            if (!eating && time_since_meal >= data->t_die)  // ← >= en vez de >
            {
                pthread_mutex_lock(&data->write_mutex);
                printf("%lld %d died\n",
                    get_timestamp() - data->start_time, i + 1);
                pthread_mutex_unlock(&data->write_mutex);

                pthread_mutex_lock(&data->death_mutex);
                data->simulation_stop = 1;
                pthread_mutex_unlock(&data->death_mutex);
                return (NULL);
            }
            i++;
        }
        usleep(500);
    }
}



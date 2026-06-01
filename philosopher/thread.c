/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+ +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:07:55 by gubusque          #+#    #+#             */
/*   Updated: 2026/06/01 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int simulation_should_stop(t_data *data)
{
    int stop = 0;

    pthread_mutex_lock(&data->death_mutex);
    stop = data->simulation_stop;
    pthread_mutex_unlock(&data->death_mutex);

    if (stop)
        return (1);

    pthread_mutex_lock(&data->full_mutex);
    if (data->must_eat > 0 && data->philos_full == data->n_philo)
        stop = 1;
    pthread_mutex_unlock(&data->full_mutex);

    return (stop);
}

void think(t_philo *p)
{
    print_action(p, "is thinking");
    // Removed fixed usleep — not needed
}

void philo_sleep(t_philo *p)
{
    print_action(p, "is sleeping");
    smart_sleep(p->data->t_sleep, p->data);
    // Removed extra usleep
}

void release_forks(t_philo *p)
{
    pthread_mutex_unlock(p->left_fork);
    pthread_mutex_unlock(p->right_fork);
}

void eat(t_philo *p)
{
    pthread_mutex_lock(&p->meal_mutex);
    p->last_meal_time = get_timestamp();
    p->is_eating = 1;
    pthread_mutex_unlock(&p->meal_mutex);

    print_action(p, "is eating");
    smart_sleep(p->data->t_eat, p->data);

    pthread_mutex_lock(&p->meal_mutex);
    p->is_eating = 0;
    pthread_mutex_unlock(&p->meal_mutex);

    pthread_mutex_lock(&p->data->full_mutex);
    p->meals_eaten++;
    if (p->data->must_eat > 0 && p->meals_eaten == p->data->must_eat && !p->is_full)
    {
        p->is_full = 1;
        p->data->philos_full++;
    }
    pthread_mutex_unlock(&p->data->full_mutex);
}

void take_forks(t_philo *p)
{
    pthread_mutex_t *first = p->left_fork;
    pthread_mutex_t *second = p->right_fork;

    if (p->id % 2 == 1)  // odd philosophers take right first
    {
        first = p->right_fork;
        second = p->left_fork;
    }

    pthread_mutex_lock(first);
    print_action(p, "has taken a fork");
    pthread_mutex_lock(second);
    print_action(p, "has taken a fork");
}

void *philo_routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    if (p->data->n_philo == 1)
    {
        pthread_mutex_lock(p->left_fork);
        print_action(p, "has taken a fork");
        while (!simulation_should_stop(p->data))
            usleep(500);
        pthread_mutex_unlock(p->left_fork);
        return (NULL);
    }

    // Small initial stagger for even philosophers
    if (p->id % 2 == 0)
        usleep(1000);   // 1ms is enough

    while (!simulation_should_stop(p->data))
    {
        take_forks(p);
        eat(p);
        release_forks(p);
        philo_sleep(p);
        think(p);
    }
    return (NULL);
}
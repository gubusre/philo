/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*             at                                   +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 18:07:55 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/20 18:08:33 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	think(t_philo *p)
{
	print_action(p,"is thinking");
	usleep(200);
}

void	philo_sleep(t_philo *p)
{
	print_action(p, "is sleeping");
	smart_sleep(p->data->t_sleep, p->data);
	usleep(200);
}

/* release_forks: desbloquear en orden inverso al que se adquirió */
void release_forks(t_philo *p)
{
    /* si el filósofo adquirió primero el fork con menor índice,
       liberamos en orden inverso al que se adquirió para mayor claridad */
    if (p->left_index < p->right_index)
    {
        pthread_mutex_unlock(p->right_fork);
        pthread_mutex_unlock(p->left_fork);
    }
    else
    {
        pthread_mutex_unlock(p->left_fork);
        pthread_mutex_unlock(p->right_fork);
    }
}

/* eat: proteger actualizaciones de meals_eaten / is_full con full_mutex */
void eat(t_philo *p)
{
    pthread_mutex_lock(&p->data->death_mutex);
    p->is_eating = 1;
    p->last_meal_time = get_timestamp();
    pthread_mutex_unlock(&p->data->death_mutex);

    print_action(p, "is eating");
    smart_sleep(p->data->t_eat, p->data);

    pthread_mutex_lock(&p->data->death_mutex);
    p->is_eating = 0;
    pthread_mutex_unlock(&p->data->death_mutex);

    /* actualizar contador de comidas y bandera de lleno bajo full_mutex */
    pthread_mutex_lock(&p->data->full_mutex);
    p->meals_eaten++;
    if (p->data->must_eat > 0
        && p->meals_eaten == p->data->must_eat
        && !p->is_full)
    {
        p->is_full = 1;
        p->data->philos_full++;
    }
    pthread_mutex_unlock(&p->data->full_mutex);
}

/* take_forks: backoff exponencial con tope y comprobaciones protegidas */
void take_forks(t_philo *p)
{
    pthread_mutex_t *first = p->left_fork;
    pthread_mutex_t *second = p->right_fork;

    if (p->left_index > p->right_index)
    {
        first = p->right_fork;
        second = p->left_fork;
    }

    int sleep_ms = 50;
    const int max_sleep_ms = 1000;

    while (!simulation_should_stop(p->data))
    {
        pthread_mutex_lock(first);
        if (simulation_should_stop(p->data))
        {
            pthread_mutex_unlock(first);
            return ;
        }
        print_action(p, "has taken a fork");

        if (pthread_mutex_trylock(second) == 0)
        {
            if (simulation_should_stop(p->data))
            {
                pthread_mutex_unlock(second);
                pthread_mutex_unlock(first);
                return ;
            }
            print_action(p, "has taken a fork");
            return ;
        }
        /* no pudo tomar el segundo, liberar y esperar con backoff */
        pthread_mutex_unlock(first);

        /* backoff exponencial con jitter por id para reducir colisiones */
        usleep((useconds_t)(sleep_ms * 1000 + (p->id % 30) * 10));
        sleep_ms *= 2;
        if (sleep_ms > max_sleep_ms)
            sleep_ms = max_sleep_ms;
    }
}


int	simulation_should_stop(t_data *data)
{
	int stop;

	stop = 0;
	pthread_mutex_lock(&data->death_mutex);
	if (data->simulation_stop == 1)
		stop = 1;
	pthread_mutex_unlock(&data->death_mutex);
	if (stop)
		return (1);
	pthread_mutex_lock(&data->full_mutex);
	if(data->must_eat> 0 && data->philos_full == data->n_philo)
		stop = 1;
	pthread_mutex_unlock(&data->full_mutex);
	return (stop);
}

void *philo_routine(void *arg)
{
    t_philo *p = (t_philo *)arg;

    if (p->data->n_philo == 1)
    {
        pthread_mutex_lock(p->left_fork);
        print_action(p, "has taken a fork");
        while (!simulation_should_stop(p->data))
            usleep(100);
        pthread_mutex_unlock(p->left_fork);
        return (NULL);
    }
    if (p->id % 2 == 1)
        smart_sleep(p->data->t_eat, p->data); 
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
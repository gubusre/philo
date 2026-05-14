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
}

void	philo_sleep(t_philo *p)
{
	print_action(p, "is sleeping");
	smart_sleep(p->data->t_sleep, p->data);
}

void	release_forks(t_philo *p)
{
	pthread_mutex_unlock(p->left_fork);
	pthread_mutex_unlock(p->right_fork);
}

void	eat(t_philo *p)
{
	p->is_eating = 1;
	pthread_mutex_lock(&p->data->death_mutex);
	p->last_meal_time = get_timestamp();
	pthread_mutex_unlock(&p->data->death_mutex);
	print_action(p,"is eating");
	smart_sleep(p->data->t_eat, p->data);
	p->meals_eaten++;
	if(p->data->must_eat > 0 && p->meals_eaten == p->data->must_eat)
	{
		pthread_mutex_lock(&p->data->full_mutex);
		p->data->philos_full++;
		pthread_mutex_unlock(&p->data->full_mutex);
	}
	p->is_eating = 0;
}

void	take_forks(t_philo *p)
{
	if (p->id % 2 == 0)
	{
		pthread_mutex_lock(p->left_fork);
		print_action(p, "has taken a fork");
		pthread_mutex_lock(p->right_fork);
		print_action(p, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(p->right_fork);
		print_action(p, "has taken a fork");
		pthread_mutex_lock(p->left_fork);
		print_action(p, "has taken a fork");
	}
}

int	simulation_should_stop(t_data *data)
{
	int stop;

	stop = 0;
	pthread_mutex_lock(&data->death_mutex);
	if (data->simulation_stop == 1)
		stop = 1;
	pthread_mutex_unlock(&data->death_mutex);;
	if (stop)
		return (1);
	pthread_mutex_lock(&data->full_mutex);
	if(data->must_eat> 0 && data->philos_full == data->n_philo)
		stop = 1;
	pthread_mutex_unlock(&data->full_mutex);
	return (stop);
}

void	*philo_routine(void *arg)
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
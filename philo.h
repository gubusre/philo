/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gubusque <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 17:57:57 by gubusque          #+#    #+#             */
/*   Updated: 2026/04/20 17:58:02 by gubusque         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>

typedef struct s_data t_data;
typedef struct s_philo t_philo;

typedef struct s_philo
{
	pthread_t thread;
	int	id;

	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;

	long long	last_meal_time;
	int		meals_eaten;
	int		is_eating;

	t_data	*data;
}	t_philo;

typedef struct s_data
{
	int		n_philo;
	int		must_eat;
	long long	t_die;
	long long	t_eat;
	long long	t_sleep;

	long long		start_time;

	pthread_mutex_t	*forks;
	pthread_mutex_t	write_mutex;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	full_mutex;

	int				simulation_stop;
	int				philos_full;
	t_philo		*philos;
} t_data;

/*thread*/
void *philo_routine(void *arg);

/* thread actions */
void    take_forks(t_philo *p);
void    eat(t_philo *p);
void    release_forks(t_philo *p);
void    philo_sleep(t_philo *p);
void    think(t_philo *p);

/* monitor */
void    *monitor(void *arg);

/* stop check */
int     simulation_should_stop(t_data *data);

/*aux*/
long long get_timestamp(void);
void	cleanup(t_data *data);
void	smart_sleep(long long duration, t_data *data);
void	print_action(t_philo *p, const char *msg);
# endif
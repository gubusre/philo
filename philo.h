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

static const int MS = 1000;

typedef struct s_data t_data;

typedef struct s_philo
{
	int	id;
	int	l_fork;
	int	r_fork;
	int	last_meal;
	int	meals_eaten;
	pthread_t	thread;
	t_data	*data;
}	t_philo;

typedef struct s_data
{
	int		n;
	long	t_die;
	long	t_eat;
	long	t_sleep;
	long	m_eat;

	long	t_start;

	pthread_mutex_t	*forks;
	pthread_mutex_t	print;
	
	t_philo		*philos;
} t_data;
# endif
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

/* ──────────────────────────────── */
/*              INCLUDES            */
/* ──────────────────────────────── */

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>

/* ──────────────────────────────── */
/*           FORWARD DECLARE        */
/* ──────────────────────────────── */

typedef struct s_data  t_data;
typedef struct s_philo t_philo;

/* ──────────────────────────────── */
/*           PHILOSOPHER            */
/* ──────────────────────────────── */

struct s_philo
{
    pthread_t       thread;          // Hilo del filósofo
    int             id;              // ID (0..n-1)

    pthread_mutex_t *left_fork;      // Puntero al tenedor izquierdo
    pthread_mutex_t *right_fork;     // Puntero al tenedor derecho

    long long       last_meal_time;  // Timestamp del último plato
    int             meals_eaten;     // Veces que ha comido
    int             is_eating;       // Flag para evitar race conditions

    t_data          *data;           // Acceso a la configuración global
};

/* ──────────────────────────────── */
/*               DATA               */
/* ──────────────────────────────── */

struct s_data
{
    int             n_philo;         // Número de filósofos
    long long       t_die;           // Tiempo máximo sin comer
    long long       t_eat;           // Tiempo comiendo
    long long       t_sleep;         // Tiempo durmiendo
    int             must_eat;        // Veces mínimas que deben comer (opcional)

    long long       start_time;      // Timestamp del inicio de la simulación

    pthread_mutex_t *forks;          // Array de mutex (tenedores)
    pthread_mutex_t write_mutex;     // Protege los prints
    pthread_mutex_t death_mutex;     // Protege la variable simulation_stop
    pthread_mutex_t full_mutex;      // Protege philos_full

    int             simulation_stop; // Flag: alguien murió o fin
    int             philos_full;     // Cuántos han alcanzado must_eat

    t_philo         *philos;         // Array de filósofos
};

/*Threads*/
void *philo_routine(void *arg);

/*Philo routine auxiliars*/
void print_action(t_philo *p, const char *msg);
void smart_sleep(long long duration, t_data *data);


#endif

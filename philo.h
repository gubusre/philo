#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>

typedef struct s_data
{
    int             n_philo;
    long long       t_die;
    long long       t_eat;
    long long       t_sleep;
    int             must_eat;

    int             forks_inited;
    int             philos_inited;
    int             threads_created;
    int             monitor_started;
    int             mutexes_inited;

    pthread_mutex_t *forks;
    pthread_mutex_t write_mutex;
    pthread_mutex_t death_mutex;
    pthread_mutex_t full_mutex;

    struct s_philo  *philos;

    long long       start_time;
    int             simulation_stop;
    int             philos_full;
}               t_data;

typedef struct s_philo
{
    int             id;
    pthread_t       thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    t_data          *data;

    pthread_mutex_t meal_mutex;
    long long       last_meal_time;
    int             meals_eaten;
    int             is_full;
    int             is_eating;
}               t_philo;

int     init_data(t_data *data, int argc, char *argv[]);
int     init_philos(t_data *data);
int     init_simulation(t_data *data);
void    cleanup(t_data *data);
void    join_threads(t_data *data);

void    *philo_routine(void *arg);
void    *monitor(void *arg);
long long get_timestamp(void);
void    smart_sleep(long long ms, t_data *data);
void    print_action(t_philo *p, const char *msg);
int     simulation_should_stop(t_data *data);

#endif

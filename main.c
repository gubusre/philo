#include "philo.h"

void join_threads(t_data *data)
{
    int i;

    i = 0;
    while (i < data->threads_created)
    {
        pthread_join(data->philos[i].thread, NULL);
        i++;
    }
}

static int is_valid_number(const char *str)
{
    int i;

    i = 0;
    if (!str || str[0] == '\0')
        return (0);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}

int init_data(t_data *data, int argc, char *argv[])
{
    int i;
    int ret;

    data->forks_inited = 0;
    data->philos_inited = 0;
    data->threads_created = 0;
    data->monitor_started = 0;
    data->mutexes_inited = 0;

    i = 1;
    while (i < argc)
    {
        if (!is_valid_number(argv[i]))
        {
            printf("Error: invalid argument '%s'\n", argv[i]);
            return (1);
        }
        i++;
    }

    data->n_philo = atoi(argv[1]);
    data->t_die   = atol(argv[2]);
    data->t_eat   = atol(argv[3]);
    data->t_sleep = atol(argv[4]);
    if (argc == 6)
        data->must_eat = atoi(argv[5]);
    else
        data->must_eat = -1;

    if (data->n_philo < 1 || data->t_die < 1 ||
        data->t_eat < 1 || data->t_sleep < 1)
        return (1);

    data->simulation_stop = 0;
    data->philos_full = 0;
    if ((ret = pthread_mutex_init(&data->write_mutex, NULL)) != 0) 
        return (1);
    if ((ret = pthread_mutex_init(&data->death_mutex, NULL)) != 0)
    {
        pthread_mutex_destroy(&data->write_mutex);
        return (1);
    }
    if ((ret = pthread_mutex_init(&data->full_mutex, NULL)) != 0)
    {
        pthread_mutex_destroy(&data->write_mutex);
        pthread_mutex_destroy(&data->death_mutex);
        return (1);
    }

    data->mutexes_inited = 1;

    data->forks = malloc(sizeof(pthread_mutex_t) * data->n_philo);
    if (!data->forks)
    {
        if (data->mutexes_inited)
        {
            pthread_mutex_destroy(&data->write_mutex);
            pthread_mutex_destroy(&data->death_mutex);
            pthread_mutex_destroy(&data->full_mutex);
            data->mutexes_inited = 0;
        }
        return (1);
    }

    for (int j = 0; j < data->n_philo; j++)
    {
        if ((ret = pthread_mutex_init(&data->forks[j], NULL)) != 0)
        {
            for (int k = 0; k < j; k++)
                pthread_mutex_destroy(&data->forks[k]);
            free(data->forks);
            if (data->mutexes_inited)
            {
                pthread_mutex_destroy(&data->write_mutex);
                pthread_mutex_destroy(&data->death_mutex);
                pthread_mutex_destroy(&data->full_mutex);
                data->mutexes_inited = 0;
            }
            return (1);
        }
    }
    data->forks_inited = 1;
    return (0);
}

int init_philos(t_data *data)
{
    int i;
    int ret;

    data->philos = malloc(sizeof(t_philo) * data->n_philo);
    if (!data->philos)
        return (1);

    i = 0;
    while (i < data->n_philo)
    {
        t_philo *p = &data->philos[i];
        p->id = i;
        p->data = data;
        p->left_fork = &data->forks[i];
        p->right_fork = &data->forks[(i + 1) % data->n_philo];
        p->meals_eaten = 0;
        p->is_full = 0;
        p->is_eating = 0;

        if (pthread_mutex_init(&p->meal_mutex, NULL) != 0)
        {
            while (--i >= 0)
                pthread_mutex_destroy(&data->philos[i].meal_mutex);
            free(data->philos);
            data->philos = NULL;
            return (1);
        }
        pthread_mutex_lock(&p->meal_mutex);
        p->last_meal_time = get_timestamp();
        pthread_mutex_unlock(&p->meal_mutex);
        i++;
    }

    data->philos_inited = 1;
    return (0);
}

int init_simulation(t_data *data)
{
    int i;
    pthread_t monitor_thread;

    data->start_time = get_timestamp();

    i = 0;
    while (i < data->n_philo)
    {
        t_philo *p = &data->philos[i];

        if (pthread_create(&p->thread, NULL, philo_routine, p) != 0)
        {
            pthread_mutex_lock(&data->death_mutex);
            data->simulation_stop = 1;
            pthread_mutex_unlock(&data->death_mutex);
            for (int j = 0; j < i; j++)
                pthread_join(data->philos[j].thread, NULL);

            return (1);
        }
        /* Mark that this thread was created so join/cleanup can be safe */
        data->threads_created++;
        i++;
    }

    if (pthread_create(&monitor_thread, NULL, monitor, data) != 0)
    {
        pthread_mutex_lock(&data->death_mutex);
        data->simulation_stop = 1;
        pthread_mutex_unlock(&data->death_mutex);

        for (int j = 0; j < data->n_philo; j++)
            pthread_join(data->philos[j].thread, NULL);

        return (1);
    }

    data->monitor_started = 1;
    pthread_detach(monitor_thread);
    return (0);
}

int main(int argc, char *argv[])
{
    t_data data;

    if (argc < 5 || argc > 6)
    {
        printf("Usage: ./philo n_philo t_die t_eat t_sleep [must_eat]\n");
        return (0);
    }
    if (init_data(&data, argc, argv) != 0)
        return (1);

    data.start_time = get_timestamp();

    if (init_philos(&data) != 0)
    {
        cleanup(&data);
        return (1);
    }
    if (init_simulation(&data) != 0)
    {
        cleanup(&data);
        return (1);
    }
    join_threads(&data);
    cleanup(&data);
    return (0);
}

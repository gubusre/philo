#include "philo.h"

long long get_timestamp(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
        return (0);
    return ((long long)tv.tv_sec * 1000LL + (long long)tv.tv_usec / 1000LL);
}

void smart_sleep(long long ms, t_data *data)
{
    long long start;

    start = get_timestamp();
    while ((get_timestamp() - start) < ms)
    {
        pthread_mutex_lock(&data->write_mutex);
        if (data->simulation_stop)
        {
            pthread_mutex_unlock(&data->write_mutex);
            break;
        }
        pthread_mutex_unlock(&data->write_mutex);
        usleep(500);
    }
}

int simulation_should_stop(t_data *data)
{
    int stop;

    pthread_mutex_lock(&data->write_mutex);
    stop = data->simulation_stop;
    pthread_mutex_unlock(&data->write_mutex);
    return (stop);
}

void cleanup(t_data *data)
{
    int i;

    if (!data)
        return;

    if (data->philos)
    {
        for (i = 0; i < data->n_philo; i++)
            pthread_mutex_destroy(&data->philos[i].meal_mutex);
        free(data->philos);
        data->philos = NULL;
    }

    if (data->forks)
    {
        for (i = 0; i < data->n_philo; i++)
            pthread_mutex_destroy(&data->forks[i]);
        free(data->forks);
        data->forks = NULL;
    }

    if (data->mutexes_inited)
    {
        pthread_mutex_destroy(&data->write_mutex);
        pthread_mutex_destroy(&data->death_mutex);
        pthread_mutex_destroy(&data->full_mutex);
        data->mutexes_inited = 0;
    }
}

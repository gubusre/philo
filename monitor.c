#include "philo.h"
#include <stdio.h>

static int all_philos_full(t_data *data)
{
    int i;
    int required;

    required = data->must_eat;
    if (required <= 0)
        return (0);
    i = 0;
    while (i < data->n_philo)
    {
        pthread_mutex_lock(&data->philos[i].meal_mutex);
        if (data->philos[i].meals_eaten < required)
        {
            pthread_mutex_unlock(&data->philos[i].meal_mutex);
            return (0);
        }
        pthread_mutex_unlock(&data->philos[i].meal_mutex);
        i++;
    }
    return (1);
}

static void set_dead_and_print(t_data *data, t_philo *p)
{
    long long time;

    pthread_mutex_lock(&data->write_mutex);
    data->simulation_stop = 1;
    time = get_timestamp() - data->start_time;
    printf("%lld %d died\n", time, p->id + 1);
    pthread_mutex_unlock(&data->write_mutex);
}

void *monitor(void *arg)
{
    t_data      *data;
    int          i;
    long long    now;
    long long    last;

    data = (t_data *)arg;
    while (1)
    {
        i = 0;
        while (i < data->n_philo)
        {
            pthread_mutex_lock(&data->philos[i].meal_mutex);
            last = data->philos[i].last_meal_time;
            pthread_mutex_unlock(&data->philos[i].meal_mutex);

            now = get_timestamp();
            if ((now - last) > data->t_die)
            {
                set_dead_and_print(data, &data->philos[i]);
                return (NULL);
            }
            i++;
        }

        if (all_philos_full(data))
        {
            pthread_mutex_lock(&data->write_mutex);
            data->simulation_stop = 1;
            pthread_mutex_unlock(&data->write_mutex);
            return (NULL);
        }
        usleep(100);
    }
    return (NULL);
}

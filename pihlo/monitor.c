





void *monitor_routine(void *arg)
{
    t_data *data = (t_data *)arg;

    while (1)
    {
        for (int i = 0; i < data->n_philo; i++)
        {
            t_philo *p = &data->philos[i];

            pthread_mutex_lock(&data->death_mutex);
            long long time_since_meal = get_timestamp() - p->last_meal_time;
            if (time_since_meal > data->t_die && p->is_eating == 0)
            {
                data->simulation_stop = 1;
                pthread_mutex_lock(&data->write_mutex);
                long long ts = get_timestamp() - data->start_time;
                printf("%lld %d died\n", ts, p->id + 1);
                pthread_mutex_unlock(&data->write_mutex);
                pthread_mutex_unlock(&data->death_mutex);
                return (NULL);
            }
            pthread_mutex_unlock(&data->death_mutex);
        }
        usleep(500);
    }
}

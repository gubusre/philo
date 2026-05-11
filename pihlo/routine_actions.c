
void smart_sleep(long long duration, t_data *data)
{
    long long start = get_timestamp();

    while (!simulation_should_stop(data))
    {
        if (get_timestamp() - start >= duration)
            break;
        usleep(500); // dormir muy poco para mantener la simulación reactiva
    }
}

void print_action(t_philo *p, const char *msg)
{
    long long timestamp;

    pthread_mutex_lock(&p->data->write_mutex);
    if (simulation_should_stop(p->data))
    {
        pthread_mutex_unlock(&p->data->write_mutex);
        return;
    }
    timestamp = get_timestamp() - p->data->start_time;
    printf("%lld %d %s\n", timestamp, p->id + 1, msg);
    pthread_mutex_unlock(&p->data->write_mutex);
}
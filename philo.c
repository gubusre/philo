#include "philo.h"

void print_action(t_philo *p, const char *msg)
{
    long long time;

    pthread_mutex_lock(&p->data->write_mutex);
    if (!p->data->simulation_stop)
    {
        time = get_timestamp() - p->data->start_time;
        printf("%lld %d %s\n", time, p->id + 1, msg);
    }
    pthread_mutex_unlock(&p->data->write_mutex);
}

static void take_forks(t_philo *p)
{
    pthread_mutex_t *first;
    pthread_mutex_t *second;

    if (p->left_fork < p->right_fork)
    {
        first = p->left_fork;
        second = p->right_fork;
    }
    else
    {
        first = p->right_fork;
        second = p->left_fork;
    }

    pthread_mutex_lock(first);
    print_action(p, "has taken a fork");
    pthread_mutex_lock(second);
    print_action(p, "has taken a fork");
}

static void release_forks(t_philo *p)
{
    pthread_mutex_unlock(p->left_fork);
    pthread_mutex_unlock(p->right_fork);
}

static void eat(t_philo *p)
{
    pthread_mutex_lock(&p->meal_mutex);
    p->last_meal_time = get_timestamp();
    pthread_mutex_unlock(&p->meal_mutex);

    print_action(p, "is eating");

    smart_sleep(p->data->t_eat, p->data);

    pthread_mutex_lock(&p->meal_mutex);
    p->meals_eaten++;
    if (p->data->must_eat > 0 && p->meals_eaten >= p->data->must_eat)
        p->is_full = 1;
    pthread_mutex_unlock(&p->meal_mutex);
}

static void philo_sleep(t_philo *p)
{
    print_action(p, "is sleeping");
    smart_sleep(p->data->t_sleep, p->data);
}

static void think(t_philo *p)
{
    print_action(p, "is thinking");
}

void *philo_routine(void *arg)
{
    t_philo *p;

    p = (t_philo *)arg;

    if (p->data->n_philo == 1)
    {
        pthread_mutex_lock(p->left_fork);
        print_action(p, "has taken a fork");
        while (!simulation_should_stop(p->data))
            usleep(500);
        pthread_mutex_unlock(p->left_fork);
        return (NULL);
    }
    if (p->id % 2 != 0)
        usleep(p->data->t_eat * 1000);
    while (!simulation_should_stop(p->data))
    {
        take_forks(p);
        if (simulation_should_stop(p->data))
        {
            release_forks(p);
            break;
        }
        eat(p);
        release_forks(p);
        philo_sleep(p);
        think(p);
    }
    return (NULL);
}
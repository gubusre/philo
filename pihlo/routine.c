






#include "philo.h"

void think(t_philo *p)
{
    print_action(p, "is thinking");
}

void sleep(t_philo *p)
{
    print_action(p, "is sleeping");
    smart_sleep(p->data->t_sleep, p->data);
}

void release_forks(t_philo *p)
{
    pthread_mutex_unlock(p->left_fork);
    pthread_mutex_unlock(p->right_fork);
}

void eat(t_philo *p)
{
    p->is_eating = 1;
    pthread_mutex_lock(&p->data->death_mutex);
    p->last_meal_time = get_timestamp();
    print_action(p, "is eating");
    smart_sleep(p->data->t_eat, p->data);
    p->meals_eaten++;
    if (p->data->must_eat > 0 && p->meals_eaten == p->data->must_eat)
    {
        pthread_mutex_lock(&p->data->full_mutex);
        p->data->philos_full++;
        pthread_mutex_unlock(&p->data->full_mutex);
    }
    p->is_eating = 0;
}

void take_forks(t_philo *p)
{
    if (p->id % 2 == 0)
    {
        pthread_mutex_lock(p->left_fork);
        print_action(p, "has taken a fork");

        pthread_mutex_lock(p->right_fork);
        print_action(p, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(p->right_fork);
        print_action(p, "has taken a fork");

        pthread_mutex_lock(p->left_fork);
        print_action(p, "has taken a fork");
    }
}

NAME    = philo

CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread

SRCS    = main.c \
          monitor.c \
          philo.c \
          utils.c \
          ft_utils.c

OBJS    = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c philo.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

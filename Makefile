CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = irc
INCLUDES = -I./include
SRC = main.cpp src/auth.cpp src/channel.cpp src/client.cpp src/server.cpp
COMMANDS = commands/PRIVMSG.cpp commands/JOIN.cpp
SRC += $(COMMANDS)
OBJS = $(SRC:.cpp=.o) $(COMMANDS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

fclean: clean
	rm -rf $(NAME)

clean:
	rm -rf $(OBJS)

re: fclean all

.PHONY: all clean fclean re

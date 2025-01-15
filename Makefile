CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = irc
INCLUDES = -I./include
SRC = main.cpp src/auth.cpp src/channel.cpp src/client.cpp src/server.cpp
COMMANDS = commands/PRIVMSG.cpp commands/JOIN.cpp commands/TOPIC.cpp commands/INVITE.cpp commands/MODE.cpp commands/KICK.cpp
SRC += $(COMMANDS)
OBJS = $(SRC:.cpp=.o) $(COMMANDS:.cpp=.o)
TOTAL_FILES = $(words $(SRC))

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "\nBuild complete!"
	@echo "\n> Run ./irc <port_number> <password> to create your irc server on <port_number>.\n> It will be accessible when entering <password>!"
	@echo "> Usually, you would run IRC on port 6667, but you can choose any port number you want!"
	@echo "> To connect, you can use any IRC client or simply run \033[32m'nc localhost <port_number>'\033[0m in your terminal!"
	@echo "\nThank you! Bye!"

%.o: %.cpp
	@$(eval PROGRESS=$(shell echo $$(($(PROGRESS)+1))))
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@printf "\033[34m\rCompiling %-20s [%s%s] %3d%%\033[0m" "$<" \
		"$$(printf '█%.0s' $$(seq 1 $$(($(PROGRESS)*50/$(TOTAL_FILES)))))" \
		"$$(printf '░%.0s' $$(seq 1 $$((50-$(PROGRESS)*50/$(TOTAL_FILES)))))" \
		"$$(( $(PROGRESS) * 100 / $(TOTAL_FILES) ))"

fclean: clean
	@rm -rf $(NAME)

clean:
	@rm -rf $(OBJS)

re: fclean all

.PHONY: all clean fclean re

PROGRESS=0

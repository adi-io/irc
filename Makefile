CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = ircserv
INCLUDES = -I./include

# Source files
SRC = main.cpp src/auth.cpp src/channel.cpp src/client.cpp src/server.cpp
COMMANDS = commands/PRIVMSG.cpp commands/JOIN.cpp commands/TOPIC.cpp commands/INVITE.cpp commands/MODE.cpp commands/KICK.cpp commands/PART.cpp commands/KICK.cpp commands/QUIT.cpp commands/PING-PONG.cpp
SRC += $(COMMANDS)

# Object files
OBJ_DIR = objects
OBJS = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
TOTAL_FILES = $(words $(SRC))

all: $(OBJ_DIR) $(NAME)

# Create objects directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/src
	@mkdir -p $(OBJ_DIR)/commands

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "\nBuild complete!"
	@echo "\n> Run ./ircserv <port_number> <password> to create your ircserv server on <port_number>.\n> It will be accessible when entering <password>!"
	@echo "> Usually, you would run IRC on port 6667, but you can choose any port number you want!"
	@echo "> To connect, you can use any IRC client or simply run \033[32m'nc localhost <port_number>'\033[0m in your terminal!"
	@echo "\nThank you! Bye!"

# Pattern rule for object files
$(OBJ_DIR)/%.o: %.cpp
	@$(eval PROGRESS=$(shell echo $$(($(PROGRESS)+1))))
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@printf "\033[34m\rCompiling %-20s [%s%s] %3d%%\033[0m" "$<" \
		"$$(printf '█%.0s' $$(seq 1 $$(($(PROGRESS)*50/$(TOTAL_FILES)))))" \
		"$$(printf '░%.0s' $$(seq 1 $$((50-$(PROGRESS)*50/$(TOTAL_FILES)))))" \
		"$$(( $(PROGRESS) * 100 / $(TOTAL_FILES) ))"

fclean: clean
	@clear
	@rm -rf $(NAME)
	@rm -rf $(OBJ_DIR)

clean:
	@rm -rf $(OBJS)

re: fclean all

.PHONY: all clean fclean re $(OBJ_DIR)

PROGRESS=0
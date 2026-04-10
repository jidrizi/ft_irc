CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

NAME = ircserv

all: $(NAME)

SRCDIR = srcs/

SRCS = $(SRCDIR)main.cpp $(SRCDIR)Server.cpp

BIN_DIR = bin

OBJS = $(SRCS:$(SRCDIR)%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRCDIR)%.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -rf $(BIN_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
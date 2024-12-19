NAME = webserv

ifeq ($(shell uname), Darwin)
    CXX = /opt/homebrew/bin/g++-14
else
    CXX = g++
endif

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
CPPFLAGS += -I./include

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS =	$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Request.cpp \
		$(SRC_DIR)/Response.cpp \
		$(SRC_DIR)/ConfigFile.cpp \
		# $(SRC_DIR)/ServerConfig.cpp \

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
NAME = webserv

CXX = g++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS += -I./include

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS =	$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Socket.cpp \
		$(SRC_DIR)/ConfigFile.cpp \
		$(SRC_DIR)/ServerConfig.cpp \
		$(SRC_DIR)/Client.cpp \
		$(SRC_DIR)/Utils.cpp \
		$(SRC_DIR)/RouteBlock.cpp \
		$(SRC_DIR)/Request.cpp \

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
# Makefile for cub3D

# Detect the operating system
OS := $(shell uname -s)

# Program name
NAME := cub3D

# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -Werror -w
ifeq ($(OS),Linux)
	CFLAGS += -D __LINUX__
endif

# Directories
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
BNS_DIR := bonus

LFT_DIR := libft

ifeq ($(OS),Linux)
	MLX_DIR := mlx_linux
else
	MLX_DIR := mlx_mac
endif

# .c files
SRCS_PATTERN := "*.c"
SRCS := $(shell find $(SRC_DIR)/ -type f -name $(SRCS_PATTERN))

# .o files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Include flags (-I flags)
INCFLAGS := $(addprefix -I,$(shell find . -type d -name "include" -exec find "{}" -type d ";"))
INCFLAGS += -I$(MLX_DIR)/

# Linker flags (-l and -L flags)
LDFLAGS := -L$(LFT_DIR)/ -L$(MLX_DIR)/
LDLIBS := -lft
ifeq ($(OS),Linux)
	LDLIBS += -lmlx_Linux -lXext -lX11 -lm -lz
else
	LDLIBS += -lmlx -framework OpenGL -framework AppKit
endif

# Default target: Build program
all: $(LFT_DIR)/libft.a $(NAME)

# Rule to build libft.a
$(LFT_DIR)/libft.a:
	@$(MAKE) -C $(LFT_DIR)

# Linking rule
$(NAME): $(OBJS)
	@$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Compilation rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

# Clean target: Removes .o files
clean:
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LFT_DIR) clean
	@$(MAKE) -C $(BNS_DIR) clean

# Full clean target: Removes .o files and program
fclean: clean
	@rm -rf $(NAME)
	@$(MAKE) -C $(LFT_DIR) fclean

# Rebuild default target: Cleans and rebuilds the entire program
re: fclean all

# Bonus target: Build with bonus features
bonus: $(BNS_DIR)/$(NAME)
	@mv $(BNS_DIR)/$(NAME) .

# Build bonus program
$(BNS_DIR)/$(NAME):
	@$(MAKE) -C $(BNS_DIR) re

# Phony target
PHONY:
	all clean fclean re bonus

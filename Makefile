OBJS = kchip-8.cpp
CC = g++
COMPILER_FLAGS = -std=c++23 -Wall -Wextra -Werror
LINKER_FLAGS = `sdl2-config --cflags --libs`
OBJ_NAME = kchip-8

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
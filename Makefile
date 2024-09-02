OBJS = kchip-8.cpp
CC = g++
COMPILER_FLAGS = -std=c++23 -Wall -Wextra -Werror
ifeq ($(OS),Windows_NT)
	LINKER_FLAGS = -L.\SDL2-2.30.7\x86_64-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2 -static -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid
	INCLUDES = -I.\SDL2-2.30.7\x86_64-w64-mingw32\include\SDL2
	OBJ_NAME = kchip-8.exe
else
	LINKER_FLAGS = `sdl2-config --cflags --libs`
	OBJ_NAME = kchip-8
endif

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDES) -o $(OBJ_NAME)
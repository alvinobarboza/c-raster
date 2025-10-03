CC = gcc
SRC = ./src
TEST = ./test
INCLUDE = ./include
BIN = ./bin
CFLAGS = -std=c99 -Wall -Wextra
DEBUG = -fsanitize=address -g3
CFLAGS_O = $(CFLAGS) -O2

include_paths = -I./external/raylib/include/
include_paths += -I./include/

lib_paths_linux = -L./external/raylib/lib-linux/

lib_paths_win = -L./external/raylib/lib-win/

dependencies_linux = -lraylib 
dependencies_linux += -lm 
dependencies_linux += -lpthread 
dependencies_linux += -ldl 
dependencies_linux += -lrt 
dependencies_linux += -lX11

dependencies_win = -lraylib
dependencies_win += -lgdi32 
dependencies_win += -lwinmm
dependencies_win += -lopengl32

bin_linux = $(BIN)/c_raster
bin_win = $(BIN)/c_raster.exe

bin_linux_test = $(BIN)/c_raster_test
bin_win_test = $(BIN)/c_raster_test.exe

source_files = $(SRC)/*.c

src_test_files = $(TEST)/*.c
src_test_files += $(SRC)/models.c
src_test_files += $(SRC)/transforms.c
src_test_files += $(SRC)/shapes.c

# Target for building the project
.PHONY: build
build: 
ifeq ($(OS),Windows_NT)
	$(CC) $(source_files) $(CFLAGS) $(include_paths) $(lib_paths_win) $(dependencies_win) -o $(bin_win) 
else
	$(CC) $(DEBUG) $(source_files) $(CFLAGS) $(include_paths) $(lib_paths_linux) $(dependencies_linux) -o $(bin_linux) 
endif

# Default target
.PHONY: run
run: build
	$(bin_linux)

# Target for building the project
.PHONY: release
release: 
ifeq ($(OS),Windows_NT)
	$(CC) $(source_files) $(CFLAGS_O) $(include_paths) $(lib_paths_win) $(dependencies_win) -o $(bin_win) 
else
	$(CC) $(source_files) $(CFLAGS_O) $(include_paths) $(lib_paths_linux) $(dependencies_linux) -o $(bin_linux) 
endif

# Target for cleaning up
.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	rm $(bin_win) $(bin_win_test)
else
	rm $(bin_linux) $(bin_linux_test)
endif

# Simple tests
.PHONY: tbuild
tbuild: 
ifeq ($(OS),Windows_NT)
	$(CC) $(src_test_files) $(CFLAGS) $(include_paths) $(lib_paths_win) $(dependencies_win) -o $(bin_win_test) 
else
	$(CC) $(DEBUG) $(src_test_files) $(CFLAGS) $(include_paths) $(lib_paths_linux) $(dependencies_linux) -o $(bin_linux_test) 
endif

# Run tests
.PHONY: test
test: tbuild
	$(bin_linux_test)
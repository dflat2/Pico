SHELL=/bin/sh

CC=gcc
FLAGS=-arch x86_64 -shared -undefined dynamic_lookup
WARN=-Wall -Wextra -pedantic -Wno-unused-parameter
SOURCE=$(shell find src/ -name '*.c')
SOURCE_DIRECTORY=./src/
INCLUDE=-I./ -I./src/
TARGET=SPCPlugin.dylib
DEBUG_FLAGS=-g -O0

release:
	@set -e
	@echo 'Generating RELEASE build...'
	@$(CC) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)

debug:
	@set -e
	@echo 'Generating DEBUG build...'
	@$(CC) $(WARN) $(DEBUG_FLAGS) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)

clean:
	rm -rf $(TARGET)

SHELL=/bin/sh

# Can be either `DEBUG` or `RELEASE`. Use `DEBUG` to show warnings and generate debugging information.
MODE=DEBUG

CC=gcc
FLAGS=-arch x86_64 -shared -undefined dynamic_lookup
WARN=-Wall -Wextra -pedantic -Wno-unused-parameter
SOURCE=$(shell find src/ -name '*.c')
SOURCE_DIRECTORY=./src/
INCLUDE=-I./ -I./src/
TARGET=SPCPlugin.dylib
DEBUG_FLAGS=-g -O0

build: $(TARGET)

$(TARGET): makefile
	@echo 'Generating $(MODE) build...'
ifeq ($(MODE),RELEASE)
	@$(CC) $(WARN) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)
else ifeq ($(MODE),DEBUG)
	@$(CC) $(DEBUG_FLAGS) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)
else
	@echo 'Invalid mode: $(MODE)'
endif

clean:
	rm -rf $(TARGET)

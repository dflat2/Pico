SHELL=/bin/sh

# Can be either `DEBUG` or `RELEASE`. `DEBUG` will disable optimizations and run the debugger.
MODE=DEBUG

# For debugging purposes.
CLASSICUBE_PATH=$(shell cat classicube-path.txt)
DEBUGGER=lldb
USERNAME=D_Flat

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
	@set -e
	@echo 'Generating $(MODE) build...'
ifeq ($(MODE),RELEASE)
	@$(CC) $(WARN) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)
else ifeq ($(MODE),DEBUG)
	@$(CC) $(DEBUG_FLAGS) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)
	@mv -v SPCPlugin.dylib $(CLASSICUBE_PATH)/plugins
	@$(DEBUGGER) $(CLASSICUBE_PATH)/ClassiCube $(USERNAME)
else
	@echo 'Invalid mode: $(MODE)'
endif

clean:
	rm -rf $(TARGET)

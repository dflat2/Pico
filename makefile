SHELL=/bin/sh
COMPILER=gcc
FLAGS=-Wall -pedantic -arch x86_64 -shared -undefined dynamic_lookup
SOURCE=$(shell find src/ -name '*.c')
SOURCE_DIRECTORY=./src/
INCLUDE=-I./ -I./src/
TARGET=SPCPlugin.dylib

all:
	@$(COMPILER) $(SOURCE) -I. -I$(SOURCE_DIRECTORY) -o $(TARGET) $(FLAGS)

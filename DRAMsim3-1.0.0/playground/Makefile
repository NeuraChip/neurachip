# Makefile for the playground

BIN = play.bin
OBJS = play.o
# CFLAGS = -Wall -Wextra -Werror -std=c99 -O3
LDLIBS = -lm -lstdc++ -L../ -ldramsim3
CC = g++

FMT_LIB_DIR=../ext/fmt/include
INI_LIB_DIR=../ext/headers
JSON_LIB_DIR=../ext/headers
ARGS_LIB_DIR=../ext/headers

INC=-I../src/ -I$(FMT_LIB_DIR) -I$(INI_LIB_DIR) -I$(ARGS_LIB_DIR) -I$(JSON_LIB_DIR)
CXXFLAGS=-Wall -O3 -fPIC -std=c++11 $(INC) -DFMT_HEADER_ONLY=1

%.o: %.c
	$(CC) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

all: $(BIN)

clean:
	rm -f $(OBJS) $(BIN)

run: $(BIN)
	./$(BIN)
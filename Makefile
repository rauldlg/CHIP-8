CFLAGS = -Wall -Wextra -fdiagnostics-color=always -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
CC = gcc
VPATH = build:src:tests

build/chip-8: chp8.c main.c
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY = run
run: chip-8
	./build/chip-8 bin/MAZE

.PHONY = debug

debug: chp8.c main.c
	$(CC) -g $^ -o tests/debug $(CFLAGS) 

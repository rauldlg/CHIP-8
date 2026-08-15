CFLAGS = -Wall -Wextra -fdiagnostics-color=always
CC = gcc
VPATH = build:src:tests

build/chip-8: chp8.c main.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY = run
run: chip-8
	./build/chip-8 bin/MAZE

.PHONY = debug

debug: chp8.c main.c
	$(CC) $(CFLAGS) -g $^ -o tests/debug


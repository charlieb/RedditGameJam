CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -D_GNU_SOURCE
INCLUDES=`sdl-config --cflags`
LIBS=-lm `sdl-config --libs` -lGL -lGLU
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
DEST=bin

.PHONY: check-syntax
check-syntax:
	$(CC) $(CFLAGS) $(INCLUDES) -fsyntax-only $(CHK_SOURCES)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

game: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o $(DEST)/game

debug: CFLAGS += -g 
debug: game

release: CFLAGS += -O2
release: game

profile: CFLAGS += -pg
profile: game

clean:
	- rm -f $(OBJECTS) $(DEST)/game
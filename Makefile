CC=gcc
LD=gcc
CFLAGS=-g -Wall -Werror -pedantic -std=c11
LIBS=-lm
# gtk+-2.0 pour GTK2
# gtk+-3.0 pour GTK3 (choisi ici)
GTKCFLAGS:=-g $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS:=$(shell pkg-config --libs gtk+-3.0)

all: convex

convex: convex.o points.o pile.o quicksort.o
	$(LD) convex.o points.o pile.o quicksort.o $(GTKLIBS) $(LIBS) -lm -o convex

convex.o: convex.c
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) convex.c -o convex.o

points.o: points.c points.h 
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) points.c -o points.o

pile.o: pile.c pile.h
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) pile.c -o pile.o

quicksort.o: quicksort.c quicksort.h
	$(CC) -c $(CFLAGS) $(GTKCFLAGS) quicksort.c -o quicksort.o

clean:
	rm -f convex convex.o points.o pile.o quicksort.o

fullclean: clean
	rm -f *~ *.fig.bak

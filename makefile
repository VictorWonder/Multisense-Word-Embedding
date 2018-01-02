CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -lpthread -lm

all: main

cmg.o: cmg.c cmg.h
	$(CC) $(CFLAGS) -c cmg.c

main.o: main.c cmg.h
	$(CC) $(CFLAGS) -c main.c

main: main.o cmg.o
	$(CC) $(CFLAGS) main.o cmg.o -o main $(LDFLAGS)

clean:
	rm -f *.o

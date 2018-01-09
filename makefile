CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result #-Werror

all: main

hash.o: hash.c cmg.h
	$(CC) -c hash.c $(CFLAGS)

vocab.o: vocab.c cmg.h
	$(CC) -c vocab.c $(CFLAGS)

kdtree.o: kdtree.c cmg.h
	$(CC) -c kdtree.c $(CFLAGS)

optimizer.o: optimizer.c cmg.h
	$(CC) -c optimizer.c $(CFLAGS)

main.o: main.c cmg.h
	$(CC) -c main.c $(CFLAGS)

main: hash.o vocab.o kdtree.o optimizer.o main.o
	$(CC) main.o hash.o vocab.o kdtree.o optimizer.o -o main $(CFLAGS)

clean:
	rm -f *.o main

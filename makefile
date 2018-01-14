CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result #-Werror

all: main

hash.o: hash.c cmg.h
	$(CC) -c hash.c $(CFLAGS)

vocab.o: vocab.c cmg.h
	$(CC) -c vocab.c $(CFLAGS)

negative.o: negative.c cmg.h
	$(CC) -c negative.c $(CFLAGS)

optimizer.o: optimizer.c cmg.h
	$(CC) -c optimizer.c $(CFLAGS)

kdtree.o: kdtree.c cmg.h
	$(CC) -c kdtree.c $(CFLAGS)

main.o: main.c cmg.h
	$(CC) -c main.c $(CFLAGS)

main: hash.o vocab.o negative.o optimizer.o kdtree.o main.o
	$(CC) main.o hash.o vocab.o negative.o optimizer.o kdtree.o -o main $(CFLAGS)

clean:
	rm -f *.o main

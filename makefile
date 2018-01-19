CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result \
	 #-I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5 #-Werror

all: main

vocab.o: vocab.c cmg.h
	$(CC) -c vocab.c $(CFLAGS)

wordvec.o: wordvec.c cmg.h
	$(CC) -c wordvec.c $(CFLAGS)

kdtree.o: kdtree.c cmg.h
	$(CC) -c kdtree.c $(CFLAGS)

helper.o: helper.c cmg.h
	$(CC) -c helper.c $(CFLAGS)

main.o: main.c cmg.h
	$(CC) -c main.c $(CFLAGS)

main: vocab.o wordvec.o kdtree.o helper.o main.o
	$(CC) main.o vocab.o wordvec.o kdtree.o helper.o -o main $(CFLAGS)

clean:
	rm -f *.o main

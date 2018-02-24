vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I../include #-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5
MAKE = make

DS = kdtree.o heap.o

all: $(DS)

kdtree.o: kdtree.c common.h ds.h
	$(CC) -c kdtree.c $(CFLAGS)

heap.o: heap.c common.h ds.h
	$(CC) -c heap.c $(CFLAGS)

clean: 
	rm -f *.o

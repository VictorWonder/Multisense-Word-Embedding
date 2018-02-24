vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result \
	 -I../include #-I/usr/local/WordNet-3.0/include/ -L/usr/local/WordNet-3.0/lib/ -lWN#-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5
MAKE = make

PSEUDO = detect.o matrix.o control.o

all: $(PSEUDO)

detect.o: detect.c common.h embed.h ds.h pseudo.h
	$(CC) -c detect.c $(CFLAGS)

matrix.o: matrix.c common.h embed.h pseudo.h
	$(CC) -c matrix.c $(CFLAGS)

control.o: control.c common.h embed.h pseudo.h
	$(CC) -c control.c $(CFLAGS)

clean: 
	rm -f *.o

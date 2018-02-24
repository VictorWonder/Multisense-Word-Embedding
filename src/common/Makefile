vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I../include #-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5
MAKE = make

COMMON = message.o math.o

all: $(COMMON)

message.o: message.c common.h
	$(CC) -c message.c $(CFLAGS)

math.o: math.c common.h
	$(CC) -c math.c $(CFLAGS)

clean: 
	rm -f *.o

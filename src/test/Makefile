vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I../include #-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5
MAKE = make

TEST = score.o test.o

all: $(TEST)

score.o: score.c common.h test.h
	$(CC) -c score.c $(CFLAGS)

test.o: test.c common.h vocab.h embed.h ds.h test.h
	$(CC) -c test.c $(CFLAGS)

clean: 
	rm -f *.o

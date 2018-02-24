vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I../include #-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5

VOCAB = hash.o vocab.o reader.o

all: $(VOCAB)

hash.o: hash.c vocab.h common.h
	$(CC) -c hash.c $(CFLAGS)

vocab.o: vocab.c vocab.h common.h
	$(CC) -c vocab.c $(CFLAGS)

reader.o: reader.c vocab.h common.h
	$(CC) -c reader.c $(CFLAGS)

clean: 
	rm -f *.o

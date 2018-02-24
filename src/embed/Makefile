vpath %.h ./../include

CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I../include #-Werror
PYTHON = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5

EMBED = wordvec.o optimizer.o sense.o model.o

all: $(EMBED)

wordvec.o: wordvec.c common.h vocab.h embed.h
	$(CC) -c wordvec.c $(CFLAGS)

optimizer.o: optimizer.c common.h vocab.h embed.h ds.h
	$(CC) -c optimizer.c $(CFLAGS)

sense.o: sense.c common.h embed.h
	$(CC) -c sense.c $(CFLAGS)

model.o: model.c common.h embed.h
	$(CC) -c model.c $(CFLAGS)


clean: 
	rm -f *.o

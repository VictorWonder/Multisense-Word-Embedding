vpath %.h ./include/
vpath %.h ./include/libxml2/include/libxml2/
VPATH = ./:./common:./vocab:./embed:./ds:./pseudo:./test

# define instructions
CC = gcc
CFLAGS = -lm -lpthread -O3 -march=native -Wall -funroll-loops -Wno-unused-result -I./include
WNFLAGS = -I/usr/local/WordNet-3.0/include/ -L/usr/local/WordNet-3.0/lib/ -lWN
XMLFLAGS = -I./include/libxml2/include/libxml2/ -lxml2
PYFLAGS = -I/usr/include/python3.5/ -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -lpython3.5
ALL_FLAGS = $(CFLAGS) $(XMLFLAGS)

MAKE = make
CLEAN = make clean

# define files
HEADER = common.h vocab.h embed.h ds.h pseudo.h test.h

COMMON = message.o math.o
VOCAB = hash.o vocab.o reader.o
EMBED = wordvec.o optimizer.o sense.o model.o
DS = kdtree.o heap.o
PSEUDO = detect.o matrix.o control.o
TEST = score.o test.o

ALL_OBJ = $(COMMON) $(VOCAB) $(EMBED) $(DS) $(PSEUDO) $(TEST)

all: core

common: $(COMMON) common.h
	cd common && $(MAKE)

vocab: $(VOCAB) common.h vocab.h
	cd vocab && $(MAKE)

embed: $(EMBED) common.h vocab.h embed.h
	cd embed && $(MAKE)

ds: $(DS) common.h ds.h
	cd ds && $(MAKE)

pseudo: $(PSEUDO) common.h embed.h ds.h pseudo.h
	cd pseudo && $(MAKE)

test: $(TEST) common.h vocab.h embed.h ds.h test.h
	cd test && $(MAKE)

core.o: core.c $(HEADER)
	$(CC) -c core.c $(ALL_FLAGS)

core: common vocab embed ds pseudo test core.o
	$(CC) -o core core.o $(ALL_OBJ) $(ALL_FLAGS)

clean:
	rm -f core *.o \
	    && cd common && $(CLEAN) \
	    && cd ../vocab && $(CLEAN) \
	    && cd ../embed && $(CLEAN) \
	    && cd ../ds && $(CLEAN) \
	    && cd ../pseudo && $(CLEAN) \
	    && cd ../test && $(CLEAN)

.PHONY: all common vocab embed ds pseudo test clean

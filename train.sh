#!/bin/sh

make
./main -dim 50 -window 5 -corpus ../data/corpus/wiki.en.corpus.crop -min-count 15 -alpha 0.005 -save-vocab ../data/cmg/vocab.txt -vector ../data/cmg/vector.txt -subsampling 0.25 -negative 5 -model 1 -thread 20
make clean

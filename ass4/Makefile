CC = gcc
CFLAGS = -g -Wall -O2

all: cache-test cache-test-reference

cache-test-reference: cache-test.o cache-reference.o
	$(CC) $(CFLAGS) cache-test.o cache-reference.o -o cache-test-reference

cache-test: cache-test.o cache.o
cache-test.o: cache-test.c cache.h
cache.o: cache.c cache.h

clean:
	-/bin/rm cache-test cache-test.o cache.o

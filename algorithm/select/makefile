.PHONY: clean

CC = gcc
CFLAGS = -Wall -O3

default: build/common.o build/qselect.o build/main.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/test $^

build/common.o: src/common/common.h src/common/common.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/common/common.c

build/main.o: src/main.c src/common/common.h src/qselect.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/main.c

build/qselect.o: src/qselect.c src/common/common.h src/qselect.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/qselect.c

clean:
	rm -f build/*
	rm -f bin/*
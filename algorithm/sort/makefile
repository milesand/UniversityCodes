.PHONY: default clean

CC = gcc
CFLAGS = -Wall -O3

default:
	@echo Usage: make SORTING-ALGORITHM

build/common.o: src/common/common.h src/common/common.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/common/common.c

build/main.o: src/main.c src/common/common.h src/sort/sort.h
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/main.c

########################
#### Insertion sort ####
########################
insertion: build/common.o build/main.o build/insertion.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $^

build/insertion.o: src/common/common.h src/sort/sort.h src/sort/insertion.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/sort/insertion.c

###################
#### Quicksort ####
###################
quick: build/common.o build/main.o build/quick.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $^

build/quick.o: src/common/common.h src/sort/sort.h src/sort/quick.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c src/sort/quick.c

###################
#### Mergesort ####
###################
merge: build/common.o build/main.o build/merge.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $^

build/merge.o: src/common/common.h src/sort/sort.h src/sort/merge.c
	$(CC) $(CFLAGS) -o $@ -c src/sort/merge.c

##################
#### Heapsort ####
##################
heap: build/common.o build/main.o build/heap.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $^

build/heap.o: src/common/common.h src/sort/sort.h src/sort/heap.c
	$(CC) $(CFLAGS) -o $@ -c src/sort/heap.c

###################
#### Shellsort ####
###################
shell: build/common.o build/main.o build/shell.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $^

build/shell.o: src/common/common.h src/sort/sort.h src/sort/shell.c
	$(CC) $(CFLAGS) -o $@ -c src/sort/shell.c

clean:
	rm -f build/*
	rm -f bin/*
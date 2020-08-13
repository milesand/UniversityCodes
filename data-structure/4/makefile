CC = gcc
CFLAGS = -Wall

default: main

main: main.o tree.o queue.o
	$(CC) $(CFLAGS) -o main main.o tree.o queue.o

main.o: main.c tree.h
	$(CC) $(CFLAGS) -c main.c

tree.o: tree.c tree.h queue.h maybe.h
	$(CC) $(CFLAGS) -c tree.c

queue.o: queue.c tree.h queue.h
	$(CC) $(CFLAGS) -c queue.c

clean:
	$(RM) main *.o
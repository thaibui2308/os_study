CC=gcc
CFLAGS=-g -Wall

countprefix: countprefix.c dictionary.c
	$(CC) $(CFLAGS) -o countprefix countprefix.c dictionary.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -c dictionary.c
clean:
	rm *.o a1
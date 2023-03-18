CC=gcc
CFLAGS=-g -Wall -pthread

countprefix: main.c populatetree.c readprefix.c countprefix.c
	$(CC) $(CFLAGS) -o countprefix main.c dictionary.c queue.c resource.c populatetree.c readprefix.c countprefix.c

populatetree.o: populatetree.c populatetree.h resource.c
	$(CC) $(CFLAGS) -c populatetree.c 

readprefix.o: readprefix.c readprefix.h resource.c
	$(CC) $(CFLAGS) -c populatetree.c 

countprefix.o: countprefix.c countprefix.h resource.c
	$(CC) $(CFLAG) -c countprefix.c

resource.o: resource.c resource.h dictionary.c queue.c
	$(CC $(CFLAGS) -c resource.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -c dictionary.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -o queue.c

clean:
	rm *.o a1
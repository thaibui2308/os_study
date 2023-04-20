CC=gcc
CFLAGS=-g -Wall -lpthread -pthread


cryptoexc: main.c producer.c consumer.c queue.c log.c cryptoexchange.c
	$(CC) $(CFLAGS) -o cryptoexc main.c producer.c consumer.c queue.c log.c cryptoexchange.c

producer.o: producer.c producer.h queue.c log.c

consumer.o: consumer.c consumer.h queue.c log.c
	$(CC) $(CFLAGS) -o consumer.c

queue.o: queue.c queue.h cryptoexchange.c
	$(CC) $(CFLAGS) -o queue.c

log.o: log.c log.h crytpoexchange.c
	$(CC) $(CFLAGS) -o log.c

cryptoexchange.o: cryptoexchange.c cryptoexchange.h
	$(CC) $(CFLAGS) -o cryptoexchange.c

clean:
	rm cryptoexc
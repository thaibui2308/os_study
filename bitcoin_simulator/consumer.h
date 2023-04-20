#ifndef CONSUMER_H
#define CONSUMER_H

#include <unistd.h>
#include "queue.h"
#include "log.h"

/*
 * Consumer abstract data structure
 */
typedef struct Consumer {
    BufferADT broker;
    ConsumerType consumer_type;
    int wait;
} Consumer;

/*
 * Pointer type to Consumer struct
 */
typedef struct Consumer *consumerPtr;

/**
 * @brief                       Consumes the requests by taking each request off 
 *                              (remove) from the trade request queue and completing
 *                              the transaction with another trader.
 * 
 * @param consume               Who removed and processed the request?
 * @return void* 
 */
void *consumer(void *consume);

/**
 * @brief                       Create a Consumer object
 * 
 * @param buffer                The Broker abstract data structure
 * @param consumer_type         Is it BlockchainX or BlockchainY consumer?  
 * @param wait_time             How long to wait for the consumption?
 * @return consumerPtr          A pointer to the consumer object 
 */
consumerPtr createConsumer(BufferADT buffer, ConsumerType consumer_type, int wait_time);


#endif

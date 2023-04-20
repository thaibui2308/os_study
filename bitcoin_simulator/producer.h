#ifndef PRODUCER_H
#define PRODUCER_H

#include "queue.h"
#include "log.h"
#include <unistd.h>

/*
 * Producer abstract data structure 
 */
typedef struct Producer {
    BufferADT broker;
    RequestType request_type;
    int wait;
} Producer;

/*
 * Pointer type to Producer struct
 */
typedef struct Producer *producerPtr;

/**
 * @brief                       Produces trade requests (accepting buy/sell trade requests from traders) 
 *                              and publish them to a request broker by
 *                              adding the requests to a trade request queue. 
 * 
 * @param produce               Who produced and added the requests?
 * @return void* 
 */
void* producer(void *produce);

/**
 * @brief Create a Producer object
 * 
 * @param buffer                The Broker abstract data structure
 * @param request_type          Is it Bitcoin or Ethereum request?  
 * @param wait_time             How long to wait for the production?
 * @return producerPtr          A pointer to the producer object 
 */
producerPtr createProducer(BufferADT buffer, RequestType request_type, int wait_timeout);

#endif
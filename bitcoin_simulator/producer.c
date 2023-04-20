#include "producer.h"

/**
 * @brief               Produces trade requests (accepting buy/sell trade requests from traders) 
 *                      and publish them to a request broker by
 *                      adding the requests to a trade request queue. 
 * 
 * @param produce       Who produced and added the requests?
 * @return void* 
 */
void* producer(void *produce) {
    Producer *producer = (Producer*) produce;
    RequestType item;

    while (1) {
        /* Producing an item */
        item = producer->request_type;

        /* Sleep for a specific amount of time */
        usleep(producer->wait * 1000);

        /* Make sure the Buffer capacity is not reached its limit */
        sem_wait(&producer->broker->availableSlots);
        
        /* Check the number of Bitcoin in the queue */
        if (item == Bitcoin) {
            sem_wait(&producer->broker->bitcoinRequests);
        }
        
        /* Access buffer exclusively */
        sem_wait(&producer->broker->mutex);
        /* If the number of total requests is reached, break */
        if (producer->broker->total_number_of_requests == 0) {
            sem_post(&producer->broker->mutex);
            break;
        }
        enqueue(producer->broker, item);
        /* Keeping track */
        producer->broker->produced_requests[item]++;
        producer->broker->requests_in_queue[item]++;
        /* Printing to the terminal */
        log_request_added(
            (RequestType) item,
            producer->broker->produced_requests,
            producer->broker->requests_in_queue
        );
        /* Decrease the number of total number of requests the service has to produced */
        producer->broker->total_number_of_requests--;
        /* Up, inform the consumer */
        sem_post(&producer->broker->mutex);
        sem_post(&producer->broker->unconsumed);
    }
    return NULL;
}

/**
 * @brief                       Create a Producer object
 * 
 * @param buffer                The Broker abstract data structure
 * @param request_type          Is it Bitcoin or Ethereum request?  
 * @param wait_time             How long to wait for the production?
 * @return producerPtr          A pointer to the producer object 
 */
producerPtr createProducer(BufferADT buffer, RequestType request_type, int wait_time) {
    producerPtr producer;
    producer = (producerPtr) malloc(sizeof(Producer));

    producer->broker = buffer;
    producer->request_type = request_type;
    producer->wait = wait_time;

    return producer;
}
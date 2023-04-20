#include "consumer.h"

/**
 * @brief                       Consumes the requests by taking each request off 
 *                              (remove) from the trade request queue and completing
 *                              the transaction with another trader.
 * 
 * @param consume               Who removed and processed the request?
 * @return void* 
 */
void* consumer(void *consume) {
    consumerPtr consumers = (consumerPtr) consume;
    RequestType item;

    while (1) {
        /* sleep for a bit */ 
        usleep(consumers->wait*1000);

        /* Block until there's something to consume */
        sem_wait(&consumers->broker->unconsumed);
        sem_wait(&consumers->broker->mutex);

        /* If the number of total requests is consumption, break */
        if (consumers->broker->total_number_of_consumptions == 0) {
            /* Signal the terminated semaphore */
            sem_post(&consumers->broker->terminated);
            break;
        }

        /* Access buffer exclusively */
        item = dequeue(consumers->broker);
        if (item == Bitcoin) { // If the consumed item is a Bitcoin, notify the producer
            sem_post(&consumers->broker->bitcoinRequests);
        }
        /* Keeping track */
        consumers->broker->requests_in_queue[item]--;
        consumers->broker->consumed_requests[item]++;
        consumers->broker->requests_type[item]++;

        /* Temporary variable for logging purpose */
        unsigned int *inRequestQueue;
        
        if (consumers->consumer_type == BlockchainX) { // Check the type of consumer
            consumers->broker->blockchain_X_consumption[item]++;
            inRequestQueue = consumers->broker->blockchain_X_consumption;
        } else {
            consumers->broker->blockchain_Y_consumption[item]++;
            inRequestQueue = consumers->broker->blockchain_Y_consumption;
        }
        consumers->broker->total_number_of_consumptions--;
        log_request_removed(
            consumers->consumer_type,
            item,
            inRequestQueue,
            consumers->broker->requests_in_queue
        );

        /* If the number of total requests is consumption, break 
         * Not a duplication instruction since we just want to be careful
         */
        if (consumers->broker->total_number_of_consumptions == 0) {
            sem_post(&consumers->broker->terminated);
            break;
        }
        /* Up, inform the producer */
        sem_post(&consumers->broker->mutex);
        sem_post(&consumers->broker->availableSlots);
    }
    return NULL;
}


/**
 * @brief                       Create a Consumer object
 * 
 * @param buffer                The Broker abstract data structure
 * @param consumer_type         Is it BlockchainX or BlockchainY consumer?  
 * @param wait_time             How long to wait for the consumption?
 * @return consumerPtr          A pointer to the consumer object 
 */
consumerPtr createConsumer(BufferADT buffer, ConsumerType consumer_type, int wait_time) {
    consumerPtr consumer;
    consumer = (consumerPtr) malloc(sizeof(Consumer));
    consumer->broker = buffer;
    consumer->wait = wait_time;
    consumer->consumer_type = consumer_type;
    return consumer;
}
#ifndef QUEUE_H
#define QUEUE_H

/*
    Queue data structure implementation based on the template provided at 
    https://www.geeksforgeeks.org/queue-linked-list-implementation/
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h>
#include "cryptoexchange.h"

#define MAX_BUFFER_SIZE 16
#define MAX_BITCOIN_REQUESTS 5
#define DEFAULT_WAITTIME 0
#define DEFAULT_R_FLAG 100


// Queue data structure implementation
typedef struct Node {
    RequestType request_type;
    struct Node *next;
} Node;

typedef struct Queue {
    struct Node *front;
    struct Node *rear;
    int size;

    /* Additional attributes to track down the number of requests produced or consumed */
    unsigned int consumed_requests[RequestTypeN];
    unsigned int produced_requests[RequestTypeN];
    unsigned int requests_in_queue[RequestTypeN];
    unsigned int requests_type[RequestTypeN];
    unsigned int blockchain_X_consumption[RequestTypeN];
    unsigned int blockchain_Y_consumption[RequestTypeN];

    /* Trackers */
    int total_number_of_requests;
    int total_number_of_consumptions;

    int isBeingConsumed;
    /* Semaphores */
    sem_t mutex, unconsumed, availableSlots, bitcoinRequests, terminated;
} Queue;

typedef struct Queue *BufferADT;

Queue* createQueue();
void enqueue(Queue *queue, RequestType request_type);
RequestType dequeue(Queue *queue);
int size(Queue *queue);
bool isEmpty(Queue *queue);

#endif

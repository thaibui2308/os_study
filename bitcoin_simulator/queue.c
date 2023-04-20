#include "queue.h"

Queue* createQueue() {
    Queue *queue = malloc(sizeof(struct Queue));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    queue->isBeingConsumed = 0;

    for (int i = 0; i < RequestTypeN; i++) {
        queue->consumed_requests[i] = 0;
        queue->produced_requests[i] = 0;
        queue->requests_in_queue[i] = 0;
        queue->requests_type[i] = 0;
        queue->blockchain_X_consumption[i] = 0;
        queue->blockchain_Y_consumption[i] = 0;
    }
    sem_init(&queue->bitcoinRequests, 0, MAX_BITCOIN_REQUESTS);
    sem_init(&queue->availableSlots, 0, MAX_BUFFER_SIZE);
    sem_init(&queue->mutex, 0, 1);
    sem_init(&queue->unconsumed, 0, 0);
    sem_init(&queue->terminated, 0, 0);
    queue->total_number_of_consumptions = 0;
    queue->total_number_of_requests = 0;
    return queue;
}

bool isEmpty(Queue *queue) {
    return queue->front == NULL ? true : false;
}

void enqueue(Queue *queue, RequestType request_type) {
    Node *newNode = malloc(sizeof(struct Node));
    newNode->request_type = request_type;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
}

RequestType dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        return -1;
    }

    struct Node *temp = queue->front;

    RequestType data = temp->request_type;

    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;

    return data;
}

int size(Queue *queue) {
    return queue->size;
}
#include "queue.h"

Queue* createQueue() {
    Queue *queue = malloc(sizeof(struct Queue));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

bool isEmpty(Queue *queue) {
    return queue->front == NULL ? true : false;
}

void enqueue(Queue *queue, char *data) {
    Node *newNode = malloc(sizeof(struct Node));
    strcpy(newNode->data, data);
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
}

char* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty\n");
        return NULL;
    }

    struct Node *temp = queue->front;

    char *data = malloc(sizeof(char) * (strlen(temp->data) + 1));
    strcpy(data, temp->data);

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

// utils
int numChars(char *filename) {
    long count = 0;
    FILE *f;

    f = fopen(filename, "r");

    if (f == NULL) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    count = ftell(f);
    fclose(f);
    return (int) count;
} 
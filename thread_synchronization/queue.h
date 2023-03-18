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

#define MAX_STRING_LENGTH 1024


// Queue data structure implementation
typedef struct Node {
    char data[MAX_STRING_LENGTH];
    struct Node *next;
} Node;

typedef struct Queue {
    struct Node *front;
    struct Node *rear;
    int size;
} Queue;

Queue* createQueue();
void enqueue(Queue *queue, char *data);
char* dequeue(Queue *queue);
int size(Queue *queue);
bool isEmpty(Queue *queue);
int numChars(char *filename);

#endif

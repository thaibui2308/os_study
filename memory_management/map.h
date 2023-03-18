#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>

/* Default variables for testing purpose only */
#define DEFAULT_NUM_LEVEL 3
#define DEFAULT_FIRST_LEVEL_BITS 4
#define DEFAULT_SECOND_LEVEL_BITS 4
#define DEFAULT_THIRD_LEVEL_BITS 12
#define MAX_NUMS_OF_BITS 28

typedef struct Map {
    void **keys;
    void **values;
    int capacity;
    int size;
} Map;

typedef struct Map* mapPtr;

/* Hash function */
unsigned int hash(void *key);

/* Initialization */
void createMap(mapPtr m, int capacity);

/* Insertion */
void insert(mapPtr m, void *key, void *value);

/* Getter */
void *get(mapPtr m, void *key);

/* Remove */
void remove(mapPtr m, void *key);



#endif
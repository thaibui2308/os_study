#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Default variables for testing purpose only */
#define DEFAULT_NUM_LEVEL 3
#define DEFAULT_FIRST_LEVEL_BITS 4
#define DEFAULT_SECOND_LEVEL_BITS 4
#define DEFAULT_THIRD_LEVEL_BITS 12
#define MIN_NUM_OF_BITS 1
#define MAX_NUMS_OF_BITS 28
#define ADDR_SIZE 32
 

typedef struct Map {
    bool isValid;
    unsigned int frame;
} Map;

typedef struct Map * mapPtr;

mapPtr createMap();





#endif
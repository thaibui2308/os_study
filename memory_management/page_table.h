#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"

typedef struct Level {
    int depth;
    bool isLeaf;
    Level** next;

} Level;

typedef struct Level * levelPtr;

typedef struct PageTable {
    levelPtr tableRootNode;
    int levelCount;
    unsigned int *bitmask;
    unsigned int *bitShift;
    int *entryCount;
} PageTable;

typedef struct PageTable * tablePtr;

#endif
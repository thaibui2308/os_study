#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"

typedef struct Level {
    void* pageTablePtr;
    int depth;
    bool isLeaf;
    Level** next;
    mapPtr map;
} Level;

typedef struct Level * levelPtr;

/* Methods for Level struct */
levelPtr createLevel(int entrycount);
void table_insert_vpn2pfn(levelPtr levelPtr, unsigned int address, unsigned int frame);

typedef struct PageTable {
    levelPtr tableRootNode;
    int levelCount;
    unsigned int *bitmask;
    unsigned int *bitShift;
    int *entryCount;
} PageTable;

typedef struct PageTable * tablePtr;

/* Methods for PageTable struct */
tablePtr createPageTable(int levelCount);
unsigned int getBitMaskForLevel(int level);
unsigned int getBitShiftForLevel(int level);
void ptab_insert_vpn2pfn(tablePtr pageTablePtr, unsigned int address, unsigned int frame);

#endif
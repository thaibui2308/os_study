#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "utils.h"
#include "map.h"

typedef struct Level {
    tablePtr pageTablePtr;
    unsigned int depth;
    bool isLeaf;
    Level** next;
    mapPtr* map;
} Level;

typedef struct Level * levelPtr;

/* Methods for Level struct */
levelPtr createLevel(tablePtr table, unsigned int depth);
void tab_insert_vpn2pfn(levelPtr levelPtr, unsigned int address, unsigned int frame);
mapPtr lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress);

typedef struct PageTable {
    levelPtr tableRootNode;
    unsigned int levelCount;
    unsigned int *bitmask;
    unsigned int *bitShift;
    unsigned int *entryCount;
} PageTable;

typedef struct PageTable * tablePtr;

/* Methods for PageTable struct */
tablePtr createPageTable(unsigned int levelCount, unsigned int *bitsAllocation);
unsigned int getBitMaskForLevel(unsigned int level);
unsigned int getBitShiftForLevel(unsigned int level);
void ptab_insert_vpn2pfn(tablePtr pageTablePtr, unsigned int address, unsigned int frame);

#endif
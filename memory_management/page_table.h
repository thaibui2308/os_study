#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include "utils.h"
#include "map.h"

typedef struct Level {
    struct PageTable *pageTablePtr;
    unsigned int depth;
    bool isLeaf;
    struct Level** next;
    mapPtr* map;
} Level;

typedef struct Level * levelPtr;
typedef struct PageTable {
    struct Level *tableRootNode;
    unsigned int levelCount;
    unsigned int *bitmask;
    unsigned int *bitShift;
    unsigned int *entryCount;

    /* Additional information to keep track of the TLB */
    unsigned int totalFrames;
    unsigned int pageHit;
    unsigned int pageMiss;
} PageTable;

typedef struct PageTable * tablePtr;

/* Methods for PageTable struct */
tablePtr createPageTable(unsigned int levelCount, unsigned int *bitsAllocation);
unsigned int getBitMaskForLevel(tablePtr pageTablePtr, int level);
unsigned int getBitShiftForLevel(tablePtr pageTablePtr, int level);
void ptab_insert_vpn2pfn(tablePtr pageTablePtr, unsigned int address, unsigned int frame);
mapPtr ptab_lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress);


/* Methods for Level struct */
levelPtr createLevel(tablePtr table, unsigned int depth);
void level_insert_vpn2pfn(levelPtr lPtr, unsigned int address, unsigned int frame);
mapPtr level_lookup_vpn2pfn(levelPtr ptr, unsigned int virtualAddress);

/* Calculate the total number of bytes the whole PageTable occupied */
void byteCount(levelPtr root, unsigned int *count);

#endif
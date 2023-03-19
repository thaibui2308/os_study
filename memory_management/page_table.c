#include "page_table.h"
#include "vaddr_tracereader.h"

/* Implementation for PageTable methods */
tablePtr createPageTable(unsigned int levelCount, unsigned int *bitsAllocation) {
    tablePtr pageTable;
    pageTable = (tablePtr) malloc(sizeof(PageTable));

    pageTable->bitmask = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);
    pageTable->bitShift = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);
    pageTable->entryCount = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);

    pageTable->levelCount = levelCount;

    unsigned int curr = 0;

    for (int i = 0; i < levelCount; i++) {
        pageTable->entryCount[i] = 1 << bitsAllocation[i];
        pageTable->bitShift[i] = ADDR_SIZE - (bitsAllocation[i]+curr);
        if (i == 0) {
            pageTable->bitmask[i] = bitmask(pageTable->bitShift[i], ADDR_SIZE);
        } else {
            pageTable->bitmask[i] = bitmask(pageTable->bitShift[i], pageTable->bitShift[i-1]);
        }
        curr = pageTable->bitShift[i];
    }

    pageTable->tableRootNode = createLevel(pageTable, 0);
    return pageTable;
}

/* Implementation for Level methods */
levelPtr createLevel(tablePtr table, unsigned int depth) {
    levelPtr level;
    level = (levelPtr) malloc(sizeof(Level));

    level->depth = depth;
    level->isLeaf = false;

    /* If this is the first level, point back to the PageTable */
    if (depth == 0) {
        level->pageTablePtr = table;
    } else {
        level->pageTablePtr = NULL;
    }

    /* If this is the leaf node, create a map instead of a next-level array */
    if (table->levelCount-1 == depth) { 
        /* Set isLeaf to true and malloc the size of Map** */
        level->isLeaf = true;
        level->map = (mapPtr*) malloc(sizeof(mapPtr)*table->entryCount[depth]);


        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->map[j] = createMap();
        }
    } else {
        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->next[j] = NULL;
        }
    }
    return level;
}

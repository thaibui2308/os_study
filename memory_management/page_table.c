#include "page_table.h"

/* Implementation for PageTable methods */
tablePtr createPageTable(unsigned int levelCount, unsigned int *bitsAllocation) {
    tablePtr pageTable;
    pageTable = (tablePtr) malloc(sizeof(PageTable));


    pageTable->totalFrames = 0;
    pageTable->totalBits = 0;
    pageTable->pageHit = 0;
    pageTable->pageMiss = 0;
    pageTable->bitmask = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);
    pageTable->bitShift = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);
    pageTable->entryCount = (unsigned int*) malloc(sizeof(unsigned int) * levelCount);

    pageTable->levelCount = levelCount;


    for (int i = 0; i < levelCount; i++) {
        pageTable->totalBits += bitsAllocation[i];
        pageTable->entryCount[i] = 1 << bitsAllocation[i];
        if (i == 0) {
            pageTable->bitShift[i] = ADDR_SIZE - bitsAllocation[i];
            pageTable->bitmask[i] = bitmask(pageTable->bitShift[i], ADDR_SIZE);
        } else {
            pageTable->bitShift[i] = pageTable->bitShift[i-1] - bitsAllocation[i];
            pageTable->bitmask[i] = bitmask(pageTable->bitShift[i], pageTable->bitShift[i-1]-1);
        }
    }

    pageTable->tableRootNode = createLevel(pageTable, 0);
    return pageTable;
}

unsigned int getBitMaskForLevel(tablePtr pageTablePtr, int level) {
    return pageTablePtr->bitmask[(int) level];
}

unsigned int getBitShiftForLevel(tablePtr pageTablePtr, int level) {
    return pageTablePtr->bitShift[(int) level];
}

void ptab_insert_vpn2pfn(tablePtr pageTablePtr, unsigned int address, unsigned int frame) {
    level_insert_vpn2pfn(pageTablePtr->tableRootNode, address, frame);
}

mapPtr ptab_lookup_vpn2pfn(PageTable *pageTable, unsigned int virtualAddress) {
    return level_lookup_vpn2pfn(pageTable->tableRootNode, virtualAddress);
}

/* Implementation for Level methods */
levelPtr createLevel(tablePtr table, unsigned int depth) {
    levelPtr level;
    level = (levelPtr) malloc(sizeof(Level));

    level->depth = depth;
    level->isLeaf = false;

    /* Set pageTablePtr pointer to the PageTable */
    level->pageTablePtr = table;
    
    /* If this is the leaf node, create a map instead of a next-level array */
    if (table->levelCount-1 == depth) { 
        /* Set isLeaf to true and malloc the size of Map** */
        level->isLeaf = true;
        level->map = (mapPtr*) malloc(sizeof(mapPtr)*table->entryCount[depth]);


        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->map[j] = createMap();
        }
    } else {
        level->next = (levelPtr*) malloc(sizeof(levelPtr)*table->entryCount[depth]);
        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->next[j] = NULL;
        }
    }
    return level;
}

void level_insert_vpn2pfn(levelPtr lPtr, unsigned int address, unsigned int frame) {
    unsigned int pNumber, totalBits;
    totalBits = lPtr->pageTablePtr->totalBits;
    Level* ptr;

    ptr = lPtr;
    
    /* While loop for inserting VPN */
    while (!ptr->isLeaf) {
        /* Find index into current page level */
        pNumber = virtualAddressToVPN(address, 
                                    ptr->pageTablePtr->bitmask[ptr->depth],
                                    ptr->pageTablePtr->bitShift[ptr->depth]);

        /* If next level is not found */
        if (ptr->next[pNumber] == NULL) {
            /* Create new level */
            ptr->next[pNumber] = createLevel(ptr->pageTablePtr, ptr->depth+1);
        }

        /* Set ptr to new level and increase depth by 1*/
        ptr = ptr->next[pNumber];
    }

    /* Processing leaf node */
    pNumber = bitmask(0, ADDR_SIZE-totalBits-1) & address;
    ptr->map[pNumber]->isValid = true;
    ptr->map[pNumber]->frame = frame;

}

mapPtr level_lookup_vpn2pfn(levelPtr lPtr, unsigned int virtualAddress) {
    levelPtr ptr;
    unsigned int pNumber;
    unsigned int totalBits = lPtr->pageTablePtr->totalBits;


    ptr = lPtr;

    /* While-loop for looking up VPN */
    while (!ptr->isLeaf) {
        pNumber = virtualAddressToVPN(virtualAddress, 
                                    ptr->pageTablePtr->bitmask[ptr->depth],
                                    ptr->pageTablePtr->bitShift[ptr->depth]);
        if (ptr->next[pNumber] == NULL) {
            return NULL;
        }
        ptr = ptr->next[pNumber];
    }

    /* Check if leaf node map contains desired entry */
    pNumber = bitmask(0, ADDR_SIZE-totalBits-1) & virtualAddress;
    if (ptr->map[pNumber]->isValid == false) {
        return NULL;
    }
    return ptr->map[pNumber];
}

/* Additional helpers */
void byteCount(levelPtr root, unsigned int *count) {
    
}

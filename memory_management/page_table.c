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
    level->current_size = 0;

    /* Set pageTablePtr pointer to the PageTable */
    level->pageTablePtr = table;
    
    /* If this is the leaf node, create a map instead of a next-level array */
    if (table->levelCount-1 == depth) { 
        /* Set isLeaf to true and malloc the size of Map** */
        level->isLeaf = true;
        level->map = (Map**) malloc(sizeof(Map) * table->entryCount[depth]);

        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->map[j] = createMap();
        }
    } else {
        level->next = (Level**) malloc(sizeof(Level*) * table->entryCount[depth]);
        for (int j = 0; j < table->entryCount[depth]; j++) {
            level->next[j] = NULL;
        }
    }
    return level;
}

void level_insert_vpn2pfn(levelPtr lPtr, unsigned int address, unsigned int frame) {
    unsigned int pNumber;
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
        ptr->current_size++;
        /* Set ptr to new level and increase depth by 1*/
        ptr = ptr->next[pNumber];
    }

    /* Processing leaf node */
    pNumber = virtualAddressToVPN(address,
                ptr->pageTablePtr->bitmask[ptr->depth],
                ptr->pageTablePtr->bitShift[ptr->depth]);

    ptr->current_size++;
    
    ptr->map[pNumber]->isValid = true;
    ptr->map[pNumber]->frame = frame;
    // printf("Success!\n\n"); 
    return;
}

mapPtr level_lookup_vpn2pfn(levelPtr lPtr, unsigned int virtualAddress) {
    levelPtr ptr;
    unsigned int pNumber;


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
    pNumber = virtualAddressToVPN(virtualAddress,
                ptr->pageTablePtr->bitmask[ptr->depth],
                ptr->pageTablePtr->bitShift[ptr->depth]);
    if (!ptr->map[pNumber]->isValid) {
        // printf("%08lx pagetable miss\n",(unsigned long int) virtualAddress);
        return NULL;
    } 
    // else {
    //     printf("%08lx pagetable hit with frame %d\n",
    //     (unsigned long int) virtualAddress,
    //     ptr->map[pNumber]->frame);
    // }
    return ptr->map[pNumber];
}

/* Additional helpers */
unsigned int sizeofPageTable(tablePtr table) {
    size_t size = 0;
    size += sizeof(PageTable);
    size += table->levelCount * sizeof(Level);
    size += table->totalBits / (sizeof(unsigned int) * 8) * sizeof(unsigned int);
    size += table->levelCount * sizeof(levelPtr);
    size += table->totalFrames * sizeof(unsigned int);
    size += sizeof(unsigned int) * 3;
    size += sizeof(Map);

    size += sizeofLevel(table->tableRootNode);

    return size;
}

unsigned int sizeofLevel(levelPtr level) {
    unsigned int entry_count = level->pageTablePtr->entryCount[level->depth];
    size_t size = 0;
    size += sizeof(Level);
    size += level->depth * sizeof(unsigned int);
    size += level->isLeaf ? sizeof(mapPtr) : sizeof(levelPtr) * (entry_count);
    size += level->isLeaf ? entry_count * sizeof(Map) : 0;

    if (!level->isLeaf) {
        for (int i = 0; i < entry_count; i++) {
            if (level->next[i] != NULL) {
                size += sizeofLevel(level->next[i]);
            }
        }
    }

    return size;
}
#include "tlb.h"

#include <stdio.h>
#include <stdlib.h>

#define TOTAL_ADDR_SIZE 32

unsigned int mask(int start, int end) {
    return ((1 << (end - start + 1)) - 1) << start;
}

int compare_tlb_entries(const void* a, const void* b) {
    TLBEntry* entry1 = (TLBEntry*)a;
    TLBEntry* entry2 = (TLBEntry*)b;
    if (entry1->last_access_time > entry2->last_access_time) {
        return -1; // entry1 comes before entry2
    } else if (entry1->last_access_time < entry2->last_access_time) {
        return 1; // entry1 comes after entry2
    } else {
        return 0; // entry1 and entry2 are equal
    }
}

void sort_tlb_cache_and_lru(TLB* tlb) {
    qsort(tlb->cache, tlb->curr_cache_cap, sizeof(TLBEntry), compare_tlb_entries);
    qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
}

TLB* createTLB(unsigned int cache_cap, unsigned int lru_cap, unsigned int vpn_size) {
    TLB* tlb = (TLB*) malloc(sizeof(TLB));
    tlb->cache_cap = cache_cap;
    tlb->curr_cache_cap = 0;
    tlb->lru_cap = lru_cap;
    tlb->curr_lru_cap = 0;
    tlb->vpn_size = vpn_size;
    tlb->vpn_shift = 0;
    
    tlb->vpn_shift = TOTAL_ADDR_SIZE-vpn_size;
    
    tlb->vpn_mask = mask(0, vpn_size-1);

    tlb->tlb_hit = 0;
    tlb->tlb_miss = 0;
    tlb->cache = (TLBEntry*) malloc(sizeof(TLBEntry) * cache_cap);
    tlb->lru = (TLBEntry*) malloc(sizeof(TLBEntry) * lru_cap);
    return tlb;
}

void freeTLB(TLB* tlb) {
    free(tlb->cache);
    free(tlb->lru);
    free(tlb);
}

unsigned int contains(TLB* tlb, unsigned int virtual_address, unsigned long curr_time, bool* vpn_found_tlb) {
    unsigned int vpn = (virtual_address >> tlb->vpn_shift) & tlb->vpn_mask;
    // printf("vpn: %08lx \t addr: %08lx\n", (unsigned long int) vpn, (unsigned long int) virtual_address);
    unsigned int physical_address = 0;
    *vpn_found_tlb = false;
    for (int i = 0; i < tlb->curr_cache_cap; i++) {
        if (tlb->cache[i].vpn == vpn) {
            physical_address = (tlb->cache[i].frame << tlb->vpn_shift) | (virtual_address & ~tlb->vpn_mask);
            tlb->cache[i].last_access_time = curr_time;
            *vpn_found_tlb = true;
            tlb->tlb_hit++;

            /* Updating the lru and cache */
            qsort(tlb->cache, tlb->curr_cache_cap, sizeof(TLBEntry), compare_tlb_entries);
            /* Check if the entry is already in LRU */
            for (int i =0; i < tlb->curr_lru_cap; i++) {
                /* If yes, update, sort, and then return */
                if (tlb->lru[i].vpn == vpn) {
                    tlb->lru[i].last_access_time = curr_time;
                    qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
                    return tlb->cache[i].frame;
                }
            }
            /* Check if LRU is full and then updating LRU */
            if (tlb->curr_lru_cap > 0) {
                qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
            }
            if (tlb->curr_lru_cap < tlb->lru_cap) {
                tlb->lru[tlb->curr_lru_cap].vpn = vpn;
                tlb->lru[tlb->curr_lru_cap].frame = tlb->cache[i].frame;
                tlb->lru[tlb->curr_lru_cap].last_access_time = curr_time;
                tlb->curr_lru_cap++;
                qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
            } else {
                tlb->lru[tlb->lru_cap-1].vpn = vpn;
                tlb->lru[tlb->lru_cap-1].frame = tlb->cache[i].frame;
                tlb->lru[tlb->lru_cap-1].last_access_time = curr_time;
                qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
            }
        }
    }
    *vpn_found_tlb = false;
    tlb->tlb_miss++;
    
    return physical_address;
}

void insertTLB(TLB* tlb, unsigned int virtual_address, unsigned int frame, unsigned long curr_time) {
    bool vpn_found_tlb = false;
    unsigned int vpn = (virtual_address >> tlb->vpn_shift) & tlb->vpn_mask;

    contains(tlb, virtual_address, curr_time, &vpn_found_tlb);
    if (!vpn_found_tlb) {
        if (tlb->curr_cache_cap > 0) {
            qsort(tlb->cache, tlb->curr_cache_cap, sizeof(TLBEntry), compare_tlb_entries);
        }
        // insert new entry
        if (tlb->curr_cache_cap < tlb->cache_cap) {
            tlb->cache[tlb->curr_cache_cap].vpn = vpn;
            tlb->cache[tlb->curr_cache_cap].frame = frame;
            tlb->cache[tlb->curr_cache_cap].last_access_time = curr_time;
            tlb->curr_cache_cap++;
            qsort(tlb->cache, tlb->curr_cache_cap, sizeof(TLBEntry), compare_tlb_entries);
        } else {
            /* Evict the last entry of the TLB after sort */
            tlb->cache[tlb->cache_cap-1].vpn = vpn;
            tlb->cache[tlb->cache_cap-1].frame = frame;
            tlb->cache[tlb->cache_cap-1].last_access_time = curr_time;
            qsort(tlb->cache, tlb->curr_cache_cap, sizeof(TLBEntry), compare_tlb_entries);
        }
        /* Check if the entry is already in LRU */
        for (int i =0; i < tlb->curr_lru_cap; i++) {
            /* If yes, update, sort, and then return */
            if (tlb->lru[i].vpn == vpn) {
                tlb->lru[i].last_access_time = curr_time;
                qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
                return;
            }
        }
        /* Check if LRU is full and then updating LRU */
        if (tlb->curr_lru_cap > 0) {
            qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
        }
        if (tlb->curr_lru_cap < tlb->lru_cap) {
            tlb->lru[tlb->curr_lru_cap].vpn = vpn;
            tlb->lru[tlb->curr_lru_cap].frame = frame;
            tlb->lru[tlb->curr_lru_cap].last_access_time = curr_time;
            tlb->curr_lru_cap++;
            qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
        } else {
            tlb->lru[tlb->lru_cap-1].vpn = vpn;
            tlb->lru[tlb->lru_cap-1].frame = frame;
            tlb->lru[tlb->lru_cap-1].last_access_time = curr_time;
            qsort(tlb->lru, tlb->curr_lru_cap, sizeof(TLBEntry), compare_tlb_entries);
        }
    }
}
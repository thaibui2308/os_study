#include "tlb.h"

#include <stdio.h>
#include <stdlib.h>

#define TOTAL_ADDR_SIZE 32

unsigned int mask(int start, int end) {
    return ((1 << (end - start + 1)) - 1) << start;
}

TLB* tlb_init(unsigned int cache_cap, unsigned int lru_cap, unsigned int vpn_size) {
    TLB* tlb = (TLB*) malloc(sizeof(TLB));
    tlb->cache_cap = cache_cap;
    tlb->curr_cache_cap = 0;
    tlb->lru_cap = lru_cap;
    tlb->curr_lru_cap = 0;
    tlb->vpn_size = vpn_size;
    tlb->vpn_shift = 0;
    
    tlb->vpn_shift = TOTAL_ADDR_SIZE-vpn_size;
    
    tlb->vpn_mask = mask(0, vpn_size-1);
    printf("shift: %08lx \t mask: %08lx\n", (unsigned long int) tlb->vpn_shift, (unsigned long int) tlb->vpn_mask);

    tlb->tlb_hit = 0;
    tlb->tlb_miss = 0;
    tlb->cache = (TLBEntry*) malloc(sizeof(TLBEntry) * cache_cap);
    tlb->lru = (TLBEntry*) malloc(sizeof(TLBEntry) * lru_cap);
    return tlb;
}

void tlb_free(TLB* tlb) {
    free(tlb->cache);
    free(tlb->lru);
    free(tlb);
}

unsigned int tlb_lookup(TLB* tlb, unsigned int virtual_address, unsigned long curr_time, bool* vpn_found_tlb) {
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
            break;
        }
    }
    if (!*vpn_found_tlb) {
        physical_address = lru_replacement_policy(tlb, vpn, curr_time);
        tlb->tlb_miss++;
    }
    return physical_address;
}

void tlb_insert(TLB* tlb, unsigned int virtual_address, unsigned int frame, unsigned long curr_time) {
    bool vpn_found_tlb;
    unsigned int vpn = (virtual_address >> tlb->vpn_shift) & tlb->vpn_mask;

    tlb_lookup(tlb, virtual_address, curr_time, &vpn_found_tlb);
    if (vpn_found_tlb) {
        // update last access time for the existing entry
        for (int i = 0; i < tlb->curr_cache_cap; i++) {
            if (tlb->cache[i].vpn == vpn) {
                tlb->cache[i].last_access_time = curr_time;
                break;
            }
        }
    } else {
        // insert new entry
        if (tlb->curr_cache_cap < tlb->cache_cap) {
            tlb->cache[tlb->curr_cache_cap].vpn = vpn;
            tlb->cache[tlb->curr_cache_cap].frame = frame;
            tlb->cache[tlb->curr_cache_cap].last_access_time = curr_time;
            tlb->curr_cache_cap++;
        } else {
            unsigned int lru_vpn = lru_replacement_policy(tlb, vpn, curr_time);
            for (int i = 0; i < tlb->curr_cache_cap; i++) {
                if (tlb->cache[i].vpn == lru_vpn) {
                    tlb->cache[i].vpn = vpn;
                    tlb->cache[i].frame = frame;
                    tlb->cache[i].last_access_time = curr_time;
                    break;
                }
            }
        }
    }
}

unsigned int lru_replacement_policy(TLB* tlb, unsigned int vpn, unsigned long curr_time) {
    unsigned int lru_vpn = tlb->lru[0].vpn;
    unsigned long lru_time = tlb->lru[0].last_access_time;
    int lru_index = 0;
    for (int i = 1; i < tlb->curr_lru_cap; i++) {
        if (tlb->lru[i].last_access_time < lru_time) {
            lru_vpn = tlb->lru[i].vpn;
            lru_time = tlb->lru[i].last_access_time;
            lru_index = i;
        }
    }
    tlb->lru[lru_index].vpn = vpn;
    tlb->lru[lru_index].last_access_time = curr_time;
    return lru_vpn;
}

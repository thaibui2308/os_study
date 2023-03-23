#ifndef TLB_H
#define TLB_H 

#include <stdlib.h>
#include <stdbool.h>

#define MAX_CAPACITY 10000 // Maximum number of entries the map can hold
typedef struct {
    unsigned int vpn;
    unsigned int frame;
    unsigned long last_access_time;
} TLBEntry;

typedef struct {
    unsigned int cache_cap;
    unsigned int curr_cache_cap;
    unsigned int lru_cap;
    unsigned int curr_lru_cap;
    unsigned int vpn_size;
    unsigned int vpn_shift;
    unsigned int vpn_mask;
    unsigned int tlb_hit;
    unsigned int tlb_miss;
    TLBEntry* cache;
    TLBEntry* lru;
} TLB;

unsigned int tlb_lookup(TLB* tlb, unsigned int virtual_address, unsigned long curr_time, bool* vpn_found_tlb);

void tlb_insert(TLB* tlb, unsigned int virtual_address, unsigned int frame, unsigned long curr_time);

unsigned int lru_replacement_policy(TLB* tlb, unsigned int vpn, unsigned long curr_time);

TLB* tlb_init(unsigned int cache_cap, unsigned int lru_cap, unsigned int vpn_size);

void tlb_free(TLB* tlb);

#endif
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CAPACITY 10000 // Maximum number of entries the map can hold

/* TLB Entry node */
typedef struct tlb_entry_t {
    unsigned int page_num; // Key
    unsigned int frame_num; // Value
} tlb_entry_t;

/* TLB table */
typedef struct tlb_t{
    tlb_entry_t *entries;
    unsigned int size;
    unsigned int capacity;
} tlb_t;

/* Methods */
tlb_t *createTLB(unsigned int capacity);
int getIndex(tlb_t *tlb, unsigned int page_num);
bool contains(tlb_t *tlb, unsigned int page_num);
unsigned int getFrameNum(tlb_t *tlb, unsigned int page_num);
void setFrameNum(tlb_t *tlb, unsigned int page_num, unsigned int frame_num);
void delete(tlb_t *tlb, unsigned int page_num);
void cleanup(tlb_t *tlb);
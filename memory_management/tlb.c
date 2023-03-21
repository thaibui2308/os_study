#include "tlb.h"

// Initialize a new TLB with the given capacity
tlb_t *createTLB(unsigned int capacity) {
    tlb_t *tlb = malloc(sizeof(tlb_t));
    tlb->entries = malloc(sizeof(tlb_entry_t) * capacity);
    tlb->size = 0;
    tlb->capacity = capacity;
    return tlb;
}

// Get the index of the TLB entry with the given page number
int getIndex(tlb_t *tlb, unsigned int page_num) {
    for (int i = 0; i < tlb->size; i++) {
        if (tlb->entries[i].page_num == page_num) {
            return i;
        }
    }
    return -1; // Page number not found in TLB
}

// Check if the TLB contains an entry with the given page number
bool contains(tlb_t *tlb, unsigned int page_num) {
    return getIndex(tlb, page_num) != -1;
}

// Get the frame number associated with the given page number in the TLB
unsigned int getFrameNum(tlb_t *tlb, unsigned int page_num) {
    int index = getIndex(tlb, page_num);
    if (index != -1) {
        return tlb->entries[index].frame_num;
    }
    return 0; // Page number not found in TLB, return default value 0
}

// Set the frame number associated with the given page number in the TLB
void setFrameNum(tlb_t *tlb, unsigned int page_num, unsigned int frame_num) {
    int index = getIndex(tlb, page_num);
    if (index != -1) {
        tlb->entries[index].frame_num = frame_num; // Page number already exists in TLB, update the frame number
    } else {
        if (tlb->size < tlb->capacity) {
            tlb->entries[tlb->size].page_num = page_num;
            tlb->entries[tlb->size].frame_num = frame_num;
            tlb->size++;
        } // If the TLB is already at capacity, do nothing
    }
}

// Remove the TLB entry with the given page number
void delete(tlb_t *tlb, unsigned int page_num) {
    int index = getIndex(tlb, page_num);
    if (index != -1) {
        // Shift all entries after the one being removed back one index to fill the gap
        for (int i = index; i < tlb->size - 1; i++) {
            tlb->entries[i] = tlb->entries[i + 1];
        }
        tlb->size--;
    }
}

// Free the memory used by the TLB
void cleanup(tlb_t *tlb) {
    free(tlb->entries);
    free(tlb);
}
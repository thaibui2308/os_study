
#include "vaddr_tracereader.h"
#include "page_table.h"
#include "print_helpers.h"
#include "tlb.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define DEFAULT_N_FLAG 0
#define DEFAULT_C_FLAG 0
#define DEFAULT_LRU_CAPACITY 8

bool n_flag_present(unsigned int flag, unsigned int current) {
    if (flag == DEFAULT_N_FLAG) {
        return true;
    }
    return current < flag;
}

int main(int argc, char **argv) {
    /* Trace filename */
    char* filename;

    /* Assign default values to N_FLAG and C_FLAG, for testing purpose only */
    int N_FLAG, C_FLAG;
    N_FLAG = DEFAULT_C_FLAG;
    C_FLAG = DEFAULT_C_FLAG;
    
    /* Default number of levels, for testing purpose only */
    unsigned int LevelCount;
    unsigned int *bitsAllocation; 

    /* IO worker */
    OutputOptionsType IO_Worker = {
        .levelbitmasks = false,
        .va2pa = false,
        .va2pa_tlb_ptwalk = false,
        .vpn2pfn = false,
        .offset = false,
        .summary = false
    };

    /* PageTable struct initialization */
    tablePtr pageTable;

    /* Parse in command line argument */

    int opt;
    while ((opt = getopt( argc, argv, "n:c:p:")) != -1) {
        switch (opt) {
            case 'n':
                N_FLAG = atoi(optarg);
                if (N_FLAG < 0) {
                    printf("Number of memory accesses must be a number, greater than or equal to 0");
                    exit(1);
                }
                break;

            case 'c':
                C_FLAG = atoi(optarg);
                if (C_FLAG < 0) {
                    printf("Cache capacity must be a number, greater than or equal to 0");
                    exit(1);
                }
                break;

            case 'p':
                if (strcmp(optarg, "levelbitmasks") == 0) {
                    IO_Worker.levelbitmasks = true;
                } else if (strcmp("va2pa",optarg) == 0) {
                    IO_Worker.va2pa = true;
                } else if (strcmp("va2pa_tlb_ptwalk",optarg) == 0) {
                    IO_Worker.va2pa_tlb_ptwalk = true;
                } else if (strcmp("vpn2pfn",optarg) == 0) {
                    IO_Worker.vpn2pfn = true;
                } else if (strcmp("offset",optarg) == 0) {
                    IO_Worker.offset = true;
                } else if (strcmp("summary", optarg) == 0) {
                    IO_Worker.summary = true;
                } else {
                    printf("Unknown printing option: %s\n",optarg);
                    exit(1);
                }
                break;

            default:
                IO_Worker.summary = true;
                N_FLAG = DEFAULT_N_FLAG;
                C_FLAG = DEFAULT_C_FLAG;
                break;

        }
    }
    if (!IO_Worker.summary && 
        !IO_Worker.va2pa && 
        !IO_Worker.levelbitmasks &&
        !IO_Worker.offset &&
        !IO_Worker.va2pa_tlb_ptwalk &&
        !IO_Worker.vpn2pfn) {
        IO_Worker.summary = true;
    }
    /* Parse in mandatory arguments */
    int pos = optind;

    filename = argv[pos];
    LevelCount = argc - (pos+1);
    bitsAllocation = (unsigned int *) malloc(sizeof(unsigned int)*LevelCount);
    int tracker, sum;
    
    if (pos < argc) {
        tracker = 0;
        sum = 0;

        for (int i = pos+1; i < argc; i++) {
            int arg = atoi(argv[i]);
            if (arg < (int) MIN_NUM_OF_BITS) {
                printf("Level %d page table must be at least %d bit\n",i,MIN_NUM_OF_BITS);
                exit(1);
            }
            sum += arg;
            bitsAllocation[tracker] = (uint32_t) arg;
            tracker++;
        }

        if (sum > MAX_NUMS_OF_BITS) {
            printf("Too many bits used in page table\n");
            exit(1);
        }
    }

    pageTable = createPageTable(LevelCount, bitsAllocation);

    /* For testing purpose only */
    if (IO_Worker.levelbitmasks) {
        report_levelbitmasks(LevelCount, pageTable->bitmask);
    } else {
        /* Open trace file */
        FILE *ifp;
        unsigned int addr_hit = 0;
        unsigned int address, physicalAddr;
        unsigned int i = 0;
        unsigned int frameCount = 0;
        p2AddrTr trace;
        bool tlbHit = false;
        bool pageTableHit = false;
        unsigned int offset, foundFrame, page_number;
        

        if ((ifp = fopen(filename,"rb")) == NULL) {
            fprintf(stderr,"cannot open %s for reading\n",argv[1]);
            exit(1);
        }

        /* Initialize TLB */
        TLB* tlb;
        if (C_FLAG == DEFAULT_C_FLAG) {
            tlb = NULL;
        } else {
            tlb = createTLB(C_FLAG, DEFAULT_LRU_CAPACITY, pageTable->totalBits); 
        }
	
        while (!feof(ifp)) {
            /* get next address and process */
            if (NextAddress(ifp, &trace) && n_flag_present(N_FLAG, i)) {
                address = trace.addr;
                /* extract offset and page number */
                page_number = bitmask(sum, ADDR_SIZE) & address;
                offset = bitmask(0, ADDR_SIZE-sum-1) & address;
                


                /* If entry is not in TLB */
                if (tlb != NULL) {
                    physicalAddr = contains(tlb, address, i, &tlbHit);
                    /* Search the TLB */
                    if (!tlbHit) {
                        tlb->tlb_miss++;
                        /* Traverse the page table */
                        mapPtr map = ptab_lookup_vpn2pfn(pageTable, address);
                        /* If entry is not in PageTable */
                        if (map == NULL) {
                            /* Insert entry to PageTable */
                            pageTable->pageMiss++;
                            ptab_insert_vpn2pfn(pageTable, address, frameCount);
                            foundFrame = frameCount;
                            /* Insert new entry to TLB and increase frameCount */
                            insertTLB(tlb, address, frameCount, i);
                            frameCount++;
                        } else {
                            pageTable->pageHit++;
                            pageTableHit = true;
                            foundFrame = map->frame;
                            insertTLB(tlb, page_number, frameCount, i);
                        
                        }
                        physicalAddr = offset + (foundFrame << (ADDR_SIZE-sum));
                    } else {
                        tlbHit = true;
                        tlb->tlb_hit++;
                        // pageTable->pageHit++;
                        physicalAddr = (physicalAddr << (ADDR_SIZE-sum)) + offset;
                    }
                }
                else {
                    mapPtr map = ptab_lookup_vpn2pfn(pageTable, address);

                    if (map == NULL) {
                        /* Insert entry to PageTable */
                        ptab_insert_vpn2pfn(pageTable, address, frameCount);
                        pageTableHit = false;
                        pageTable->pageMiss++;  
                        foundFrame = frameCount;
                        frameCount++;
                    } else {
                        pageTable->pageHit++;
                        pageTableHit = true;    
                        foundFrame = map->frame;  
                    }
                    physicalAddr = offset + (foundFrame << (ADDR_SIZE-sum));
                }
                if (IO_Worker.va2pa_tlb_ptwalk) {
                    report_va2pa_TLB_PTwalk(address, physicalAddr, tlbHit, pageTableHit);
                } else if (IO_Worker.va2pa) {
                    report_virtualAddr2physicalAddr(address, physicalAddr);
                } else if (IO_Worker.vpn2pfn) {
                    uint32_t *pages;
                    pages = (uint32_t *) malloc(sizeof(uint32_t) * LevelCount);

                    for (int j = 0; j < LevelCount; j++) {
                        pages[j] = virtualAddressToVPN(address, 
                                                    pageTable->bitmask[j],
                                                    pageTable->bitShift[j]);
                    }

                    report_pagetable_map(LevelCount, pages, foundFrame);
                } else if (IO_Worker.offset) {
                    hexnum(offset);
                }
                i++;
            }
        }	
        if (IO_Worker.summary) {
            addr_hit = pageTable->pageHit+pageTable->pageMiss;
            addr_hit += tlb == NULL ? 0 : tlb->tlb_hit;
            unsigned int num_bytes = sizeofPageTable(pageTable);
            unsigned int cacheHit = tlb == NULL ? 0 : tlb->tlb_hit;
            report_summary(1 << (ADDR_SIZE-pageTable->totalBits),
                            cacheHit,
                            pageTable->pageHit,
                            addr_hit,
                            frameCount,
                            num_bytes);
            exit(0);
        }

        /* clean up and return success */
        fclose(ifp);
    }

    return 0;
}
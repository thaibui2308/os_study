#include "vaddr_tracereader.h"
#include "page_table.h"
#include "print_helpers.h"
#include "tlb.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define DEFAULT_N_FLAG 150
#define DEFAULT_C_FLAG 0
#define DEFAULT_LRU_CAPACITY 8


int main(int argc, char **argv) {
    /* Trace filename */
    char* filename;

    /* Assign default values to N_FLAG and C_FLAG, for testing purpose only */
    int N_FLAG, C_FLAG;
    
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
                break;

            case 'c':
                C_FLAG = atoi(optarg);
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
            if (atoi(argv[i]) < MIN_NUM_OF_BITS) {
                printf("Level %d page table must be at least %d bit\n",i,MIN_NUM_OF_BITS);
                exit(1);
            }
            sum += atoi(argv[i]);
            bitsAllocation[tracker] = atoi(argv[i]);
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
        printf("trace filename: %s\n",filename);
        printf("-n is set to: %d\n",N_FLAG );
        printf("-c is set to: %d\n",C_FLAG);
        
        report_levelbitmasks(LevelCount, pageTable->bitmask);
    } else {
        /* Open trace file */
        FILE *ifp;
        unsigned int address;
        unsigned int i = 0;
        unsigned int frameCount = 0;
        p2AddrTr trace;
        

        if ((ifp = fopen(filename,"rb")) == NULL) {
            fprintf(stderr,"cannot open %s for reading\n",argv[1]);
            exit(1);
        }

        /* Initialize TLB */
        TLB* tlb;
        if (C_FLAG <= 0) {
            tlb = NULL;
        } else {
            tlb = tlb_init(C_FLAG, DEFAULT_LRU_CAPACITY, pageTable->totalBits); 
        }
	
        while (!feof(ifp)) {
            unsigned int offset, foundFrame, page_number;

            /* get next address and process */
            if (NextAddress(ifp, &trace) && i < N_FLAG) {
                bool tlbHit = false;
                bool pageTableHit = false;
                address = trace.addr;

                /* extract offset and page number */
                page_number = bitmask(sum, ADDR_SIZE) & address;
                offset = bitmask(0, ADDR_SIZE-sum-1) & address;
                
                /* Search the TLB */
                unsigned int physicalAddr = tlb_lookup(tlb, address, i, &tlbHit);

                /* If entry is not in TLB */
                if (!tlbHit) {
                    tlb->tlb_miss++;
                    /* Traverse the page table */
                    mapPtr map = ptab_lookup_vpn2pfn(pageTable, address);
                    /* If entry is not in PageTable */
                    if (map == NULL) {
                        /* Insert entry to PageTable */
                        ptab_insert_vpn2pfn(pageTable, address, frameCount);
                        pageTable->pageMiss++;

                        /* Insert new entry to TLB and increase frameCount */
                        tlb_insert(tlb, address, frameCount, i);
                        
                    } else {
                        pageTable->pageHit++;
                        pageTableHit = true;

                        tlb_insert(tlb, page_number, frameCount, i);
                        
                    }
                    foundFrame = frameCount;
                    physicalAddr = offset + (foundFrame << (ADDR_SIZE-sum));
                    frameCount++;
                } else {
                    tlbHit = true;
                    tlb->tlb_hit++;
                }
                // report_va2pa_TLB_PTwalk(address, physicalAddr, tlbHit, pageTableHit);
                report_virtualAddr2physicalAddr(address, physicalAddr);
                printf("\t \t \t \t \t \t %d\n", pageTableHit);
                i++;
            }
        }	
        
        free(tlb);

        /* clean up and return success */
        fclose(ifp);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "vaddr_tracereader.h"
#include "page_table.h"
#include "print_helpers.h"

#define DEFAULT_N_FLAG 150
#define DEFAULT_C_FLAG 0

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

    for (int i = 0; i < LevelCount; i++) { 
        printf("Bits allocated for level %d: %d\n", i, *(bitsAllocation+i));
    }

    pageTable = createPageTable(LevelCount, bitsAllocation);

    /* For testing purpose only */
    if (IO_Worker.levelbitmasks) {
        printf("trace filename: %s\n",filename);
        printf("-n is set to: %d\n",N_FLAG );
        printf("-c is set to: %d\n",C_FLAG);
        for (int i = 0; i < LevelCount; i++) {
            report_levelbitmasks(i, pageTable->bitmask+i);
        }
    }

    return 0;
}
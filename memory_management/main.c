#include "vaddr_tracereader.h"
#include "page_table.h"
#include "print_helpers.h"
// #include "tlb.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

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

    pageTable = createPageTable(LevelCount, bitsAllocation);

    for (int i = 0; i < LevelCount; i++) { 
        printf("Bits Shift for level %d: %d\n", i, pageTable->bitShift[i]);
    }

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
	
        while (!feof(ifp)) {
            /* get next address and process */
            if (NextAddress(ifp, &trace) && i < N_FLAG) {
                address = trace.addr;
                printf("%d. %08lx\n", i+1,(unsigned long int) address);
                /**
                 * @todo create a frame tracker
                 * initialize the tlb
                 * insert new address into the PageTable and tlb
                 */
                ptab_insert_vpn2pfn(pageTable, address, frameCount);
                frameCount++;
                i++;
            }
        }	

        /* clean up and return success */
        printf("%d\n", frameCount);
        fclose(ifp);
    }

    return 0;
}
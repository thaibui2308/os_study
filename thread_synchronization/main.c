#include <stdio.h>
#include <stdlib.h>
#include "populatetree.h"
#include "readprefix.h"
#include "countprefix.h"

int main(int argc, char **argv) {
    SHARED_DATA SHARED;
    SHARED = initShared();    
    
    // Initialize three threads
    pthread_t _populatetree,_readprefix,_countprefix; 

    if (argc == 0 || argc < 3) {
        perror("Error: invalid or insufficient arguments provided!");
        return 0;
    } else {
        // Populate file-related fields
        SHARED->filePath[DICTSRCFILEINDEX] = *(argv+1);
        SHARED->filePath[TESTFILEINDEX] = *(argv+2);       
        SHARED->totalNumOfCharsInFile[DICTSRCFILEINDEX] = numChars(SHARED->filePath[DICTSRCFILEINDEX]);
        SHARED->totalNumOfCharsInFile[TESTFILEINDEX] = numChars(SHARED->filePath[TESTFILEINDEX]); 

        // flags parser
        int opt = 0;

        while ((opt = getopt(argc, argv, "p:h:n:")) != -1) {
            switch (opt)
            {
            case 'p':
                SHARED->numOfProgressMarks = atoi(optarg);
                if (SHARED->numOfProgressMarks < 10) {
                    perror("Number of progress marks must be a number and at least 10");
                    exit(1);
                }
                break;
            case 'h':
                SHARED->hashmarkInterval = atoi(optarg);
                if (SHARED->hashmarkInterval <= 0 || SHARED->hashmarkInterval > 10) {
                    perror("Hash mark interval for progress must be a number, greater than 0, and less than or equal to 10");
                    exit(1);
                }
                break;
            case 'n':
                SHARED->minNumOfWordsWithAPrefixForPrinting = atoi(optarg);
                break;
            default:
                break;
            }
        }

        // Output file
        SHARED->outputFile = fopen("countprefix_output.txt", "w+");
    }
    pthread_mutex_init(&SHARED->queue_mutex, NULL);
    pthread_cond_init(&SHARED->cond_read_prefix, NULL);
    pthread_cond_init(&SHARED->cond_count_prefix, NULL);

    pthread_create(&_populatetree, NULL, populateTree, SHARED);
    pthread_create(&_readprefix, NULL, readPrefix, SHARED);
    pthread_create(&_countprefix, NULL, countPrefix, SHARED);


    // Progress tracker for populatetree
    int hashFlag = 1;
    int maxHash = 0;
    int prev = 0;
    int percentage = 0;
    int threshold = 100/SHARED->numOfProgressMarks;

    while (SHARED->taskCompleted[DICTSRCFILEINDEX] == false) {
        percentage = (int) (100*SHARED->numOfCharsReadFromFile[DICTSRCFILEINDEX]) / SHARED->totalNumOfCharsInFile[DICTSRCFILEINDEX];
        if (percentage-prev >= threshold && maxHash < SHARED->numOfProgressMarks / SHARED->hashmarkInterval) {
            if (hashFlag > 0 && hashFlag % SHARED->hashmarkInterval == 0) {
                maxHash++;
                printf("#");
            } else {
                printf("-");
            }
            prev = percentage;
            hashFlag++;
        }
    }

    // Wait for threads to finish
    pthread_join(_populatetree, NULL);
    pthread_join(_readprefix, NULL);
    pthread_join(_countprefix, NULL);
    
    
    // cleanup 
    fclose(SHARED->outputFile);
    pthread_mutex_destroy(&SHARED->queue_mutex);
    pthread_cond_destroy(&SHARED->cond_read_prefix);
    pthread_cond_destroy(&SHARED->cond_count_prefix);
    
    return 0;
}
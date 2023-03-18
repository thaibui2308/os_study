#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "dictionary.h"
#include "queue.h"

#define MAX_STRING_LENGTH 1024
#define NCHILD 30 /* a-z, ', -, _, terminator of word */ 
#define MAX_BUFFER_LENGTH 100
#define NUMOFFILES 2 
#define DICTSRCFILEINDEX 0 
#define TESTFILEINDEX 1 
 
#define DEFAULT_NUMOF_MARKS 50 
 
#define MIN_NUMOF_MARKS 10 
 
#define DEFAULT_HASHMARKINTERVAL  10 
 
#define DEFAULT_MINNUM_OFWORDS_WITHAPREFIX  1 

typedef struct RESOURCE
{
    FILE* outputFile;
    node dictRootNode; 
 
    int numOfProgressMarks; 
    int hashmarkInterval; 
 
    int minNumOfWordsWithAPrefixForPrinting;  
  
    char  *filePath[NUMOFFILES]; 
   
  
    int  totalNumOfCharsInFile[NUMOFFILES]; 

    int numOfCharsReadFromFile[NUMOFFILES]; 
 
 
    int  wordCountInFile[NUMOFFILES]; 
 
  /** 
   * number of prefixes that have been read from the prefix queue, 
   * searched and printed 
   * this is used for tracking the progress of the countprefix thread 
   */  
    int numOfProcessedPrefixes; 

    Queue* prefixQueue;

    pthread_cond_t cond_read_prefix;
    pthread_cond_t cond_count_prefix;
    pthread_mutex_t queue_mutex; 
 
    bool  taskCompleted[NUMOFFILES];  
}  RESOURCE;

typedef struct RESOURCE *SHARED_DATA;

SHARED_DATA initShared();

#endif

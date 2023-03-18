#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>


typedef enum {FALSE, TRUE} boolean_t;
typedef int el_t;


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

// Dictionary data structure implementation
typedef struct dictNode  
{  
    struct dictNode *next[NCHILD];  
} dictNode; 

typedef struct dictNode *node;

bool add(dictNode* root, char * wordBeingInserted);
dictNode* findEndingNodeOfAStr(dictNode* dictnode, char *strBeingSearched);
void countWordsStartingFromANode(dictNode *dictnode, int *count); 

node createNode();
bool isTerminated(dictNode *dictnode);


#endif
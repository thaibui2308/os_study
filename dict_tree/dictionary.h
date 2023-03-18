#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef enum {FALSE, TRUE} boolean_t;
typedef int el_t;


#define NCHILD 30 /* a-z, ', -, _, terminator of word */ 
#define MAX_BUFFER_LENGTH 1024

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
#include "dictionary.h"

#define SINGLE_QUOTE 39

node createNode() 
{
    node temp;
    temp = (node) malloc(sizeof(dictNode));
    for (int i = 0; i <NCHILD;i++)
    {
        temp->next[i] = NULL;
    }
    return temp;
}

bool add(node dictnode, char * wordBeingInserted)
{
    node location;
    char* addrPtr;

    addrPtr = wordBeingInserted;
    location = dictnode;
    if (*addrPtr != '\0') {
        int ascii = 0;
        if (*addrPtr >= 65 && *addrPtr <= 90) {
            ascii = (int) *addrPtr-'A';
        } else if (*addrPtr >= 97 && *addrPtr <= 122) {
            ascii = (int) *addrPtr-'a';
        } else if (*addrPtr == SINGLE_QUOTE){
            ascii = 26;
        } else if (*addrPtr == '-') {
            ascii = 27;
        } else if (*addrPtr == '_') {
            ascii = 28;
        } 
        if (dictnode->next[ascii] == NULL) {
            dictnode->next[ascii] = createNode();
        }
        addrPtr++;
        location = dictnode->next[ascii];
        return add(location, addrPtr);
    } 
    else {
        dictnode->next[29] = createNode();
        return true;
    }
    return false;
}

bool isTerminated(dictNode *dictnode) {
    for (int i = 0; i < NCHILD; i++) {
        if (dictnode->next[i] != NULL) {
            return false;
        }
    }
    return true;
}

dictNode* findEndingNodeOfAStr(dictNode* dictnode, char *strBeingSearched) {
    int counter = 0;
    node location;
    char* addrPtr;

    addrPtr = strBeingSearched;
    location = dictnode;
    
    while (*addrPtr != '\0') {
        int ascii = 0;
        if (*addrPtr >= 65 && *addrPtr <= 90) {
            ascii = (int) *addrPtr-'A';
        } else if (*addrPtr >= 97 && *addrPtr <= 122) {
            ascii = (int) *addrPtr-'a';
        } else if (*addrPtr == SINGLE_QUOTE){
            ascii = 26;
        } else if (*addrPtr == '-') {
            ascii = 27;
        } else if (*addrPtr == '_') {
            ascii = 28;
        }

        if (location->next[ascii]==NULL) {
            break;
        } else {
            location = location->next[ascii];
            counter++;
            addrPtr++;
        }
    }

    return counter == strlen(strBeingSearched) ? location : NULL;
}

void countWordsStartingFromANode(dictNode *dictnode, int *count) {

    // base case: if the node is '\0'
    if (isTerminated(dictnode)) {
        (*count)++;
    }

    // recursive case
    for (int i = 0; i < NCHILD; i++) {
        // if the child is not empty, proceed to traverse to the leaf node of the that branch
        if (dictnode->next[i] != NULL) {
            countWordsStartingFromANode(dictnode->next[i], count);
        }
    }
}
